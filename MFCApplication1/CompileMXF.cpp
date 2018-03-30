#include "stdafx.h"
#include "CompileMXF.h"
#include <conio.h>

CCompileAudioMXF::CCompileAudioMXF()
{
	m_fpReadFile = nullptr;
	m_fpTemplateFile = nullptr;
	m_chHeadMemory = nullptr;
	m_chAudioMixMemory = nullptr;
	m_ui64HeaderSize = 266215+25;
	m_ui64AudioLen = 1920;
	m_ui64AudioMixSize = m_ui64AudioLen * 7;
	m_ui64AudioCriSize = m_ui64AudioLen * 5;
	m_ui64AudioUseSize = 0;
	m_chAudioMixMemory = new char[m_ui64AudioMixSize];
	m_chHeadMemory = new char[m_ui64HeaderSize];
	m_chFooterMemory = new char[577];

	m_ui64AudioChannel = 8;
	m_ui64FrameCount = 0;

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		memset(m_byteTrack[i], 0x00, 8);
		m_fpOutFile[i] = nullptr;
	}
	//m_byteTimelineID[0][]= { 0x00,0x00,0x00,0x03 };
}

CCompileAudioMXF::~CCompileAudioMXF()
{
	for (int i = 0; i < m_ui64AudioChannel;i++)
	{
		SAFE_CLOSE(m_fpOutFile[i]);
	}
}
void CCompileAudioMXF::RandNumberConvByte(uint64_t in_ui64Number, BYTE *in_byte)
{
	for (int i = 7; i >= 0;i--)
	{
		in_byte[i] = (BYTE)(in_ui64Number & 0x00000000000000ff);
		in_ui64Number >>= 8;
	}
}
void CCompileAudioMXF::GetRandomNumber()
{
	srand(time(0));
	uint64_t irand = rand();
	_cprintf("Random = %lld\n",irand);

	RandNumberConvByte(irand,m_bytePackID);
	irand += 1;
	RandNumberConvByte(irand, m_byteSourceID);
	irand += 1;

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		RandNumberConvByte(irand, m_byteTrack[i]);
		irand += 1;
	}
}
bool CCompileAudioMXF::SetRWMXFPath(char *in_chReadPath, char *in_chOutPath)
{
	errno_t er = fopen_s(&m_fpReadFile, in_chReadPath, "rb+");
	if (er != 0) 
	{
		_cprintf("fopen_s ReadPath faile\n");
		return false;
	}
	m_fpTemplateFile = _fsopen("C:\\DNxHR.mxf", "rb+", SH_DENYNO);
	if (nullptr == m_fpTemplateFile)
	{
		_cprintf("_fsopen DNxHR faile\n");
		return false;
	}
	GetRandomNumber();
	char chPath[MAX_PATH] = {0};
	//
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		sprintf_s(chPath, "%s\\%0x%0x%0x%0x%0x%0x%0x%0x%s", in_chOutPath, m_byteTrack[i][0],
			m_byteTrack[i][1], m_byteTrack[i][2], m_byteTrack[i][3], m_byteTrack[i][4],
			m_byteTrack[i][5], m_byteTrack[i][6], m_byteTrack[i][7], ".mxf");

		_cprintf("chPath %d= %s\n",i, chPath);

		er = fopen_s(&m_fpOutFile[i], chPath, "wb+");
		if (er != 0)
		{
			_cprintf("fopen_s OutPath %d faile\n",i);
			return false;
		}

		memset(chPath, 0, MAX_PATH);
	}
	return true;
}
bool CCompileAudioMXF::CompileHeader()
{
	fread_s(m_chHeadMemory, m_ui64HeaderSize, 1, m_ui64HeaderSize, m_fpTemplateFile);

	for (int i = 0; i < m_ui64AudioChannel;i++)
	{
		fwrite(m_chHeadMemory, 1, m_ui64HeaderSize, m_fpOutFile[i]);
	}
	return true;
}
bool CCompileAudioMXF::CompileAudioData(char *in_chAudio, uint64_t in_ui64Len)
{
	if (in_ui64Len != 1920)
	{
		printf("CompileAudioData faile\n");
		return false;
	}
	m_ui64FrameCount++;

	memcpy_s(m_chAudioMixMemory + m_ui64AudioUseSize, m_ui64AudioMixSize, in_chAudio, in_ui64Len);
	m_ui64AudioUseSize += in_ui64Len;

	if (m_ui64AudioUseSize > m_ui64AudioCriSize)
	{
		for (int i = 0; i < m_ui64AudioChannel; i++)
		{
			fwrite(m_chAudioMixMemory, 1, m_ui64AudioUseSize, m_fpOutFile[i]);
		}
		m_ui64AudioUseSize = 0;
	}
	return true;
}
bool CCompileAudioMXF::CompileFooter()
{
	_fseeki64(m_fpReadFile, 725024, SEEK_SET);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i], 0, SEEK_END);
		m_ui64FooterPosition[i] = _ftelli64(m_fpOutFile[i]);
	}
	fread_s(m_chFooterMemory, 577, 1, 577, m_fpReadFile);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i], 0, SEEK_END);
		fwrite(m_chFooterMemory, 1, 577, m_fpOutFile[i]);
	}

	return true;
}
bool CCompileAudioMXF::CompilePosition()
{
	BYTE byteFrameCount[8];
	BYTE byteDuration[8];

	RandNumberConvByte(m_ui64FrameCount, byteFrameCount);
	RandNumberConvByte(m_ui64FrameCount*960, byteDuration);

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i], 120230, SEEK_SET);//Sequenc
		fwrite(byteFrameCount, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 120363, SEEK_SET);//Source Clip
		fwrite(byteFrameCount, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 120579, SEEK_SET);//Sequenc	1D703
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 120712, SEEK_SET);//Source Clips	1D788
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 120928, SEEK_SET);//Sequenc		1D860
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121061, SEEK_SET);//Source Clips	1D8E5
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121277, SEEK_SET);//Sequenc		1D9BD
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121410, SEEK_SET);//Source Clips	1DA42
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121626, SEEK_SET);//Sequenc		1DB1A
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121759, SEEK_SET);//Source Clips	1DB9F
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 121975, SEEK_SET);//Sequenc		1DC77
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i],122108 , SEEK_SET);//Source Clips	1DCFC
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 122324, SEEK_SET);//Sequenc		1DDD4
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 122457, SEEK_SET);//Source Clips	1DE59
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 122673, SEEK_SET);//Sequenc		1DF31
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i],122806 , SEEK_SET);//Source Clips	1DFB6
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 123022, SEEK_SET);//Sequenc		1E08E
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 123155, SEEK_SET);//Source Clips	1E113
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 124080, SEEK_SET);//Wave		1E4B0
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 124288, SEEK_SET);//Sequenc	1E580
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 124421, SEEK_SET);//Source Clips	1E605
		fwrite(byteDuration, 1, 8, m_fpOutFile[i]);
	}
	
	return true;
}
bool CCompileAudioMXF::CompilePackageUID()
{
	BYTE byteTimelineTrack[] = { 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		//Material Package
		_fseeki64(m_fpOutFile[i], 88377, SEEK_SET);
		fwrite(m_bytePackID, 1, 16, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 118822, SEEK_SET);
		fwrite(m_bytePackID, 1, 16, m_fpOutFile[i]);
		//Source Package 
		_fseeki64(m_fpOutFile[i], 88425, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 16, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 118498, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 16, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 123380, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 16, m_fpOutFile[i]);
		//SourceClip
		_fseeki64(m_fpOutFile[i], 120343, SEEK_SET);
		fwrite(m_byteSourceID, 1, 16, m_fpOutFile[i]);
		//SourceClip	A01
		_fseeki64(m_fpOutFile[i], 120692, SEEK_SET);
		fwrite(m_byteTrack[0], 1, 16, m_fpOutFile[i]);
		//SourceClip	A02
		_fseeki64(m_fpOutFile[i], 121041, SEEK_SET);
		fwrite(m_byteTrack[1], 1, 16, m_fpOutFile[i]);
		//SourceClip	A03
		_fseeki64(m_fpOutFile[i], 121390, SEEK_SET);
		fwrite(m_byteTrack[2], 1, 16, m_fpOutFile[i]);
		//SourceClip	A04
		_fseeki64(m_fpOutFile[i], 121739, SEEK_SET);
		fwrite(m_byteTrack[3], 1, 16, m_fpOutFile[i]);
		//SourceClip	A05
		_fseeki64(m_fpOutFile[i], 122088, SEEK_SET);
		fwrite(m_byteTrack[4], 1, 16, m_fpOutFile[i]);
		//SourceClip	A06
		_fseeki64(m_fpOutFile[i], 122437, SEEK_SET);
		fwrite(m_byteTrack[5], 1, 16, m_fpOutFile[i]);
		//SourceClip	A07
		_fseeki64(m_fpOutFile[i], 122786, SEEK_SET);
		fwrite(m_byteTrack[6], 1, 16, m_fpOutFile[i]);
		//SourceClip	A08
		_fseeki64(m_fpOutFile[i], 123135, SEEK_SET);
		fwrite(m_byteTrack[7], 1, 16, m_fpOutFile[i]);
		////Timeline Track
		_fseeki64(m_fpOutFile[i], 124204, SEEK_SET);
		fwrite(byteTimelineTrack+i, 1,1, m_fpOutFile[i]);
		//SourceClip SourceTrackID
		_fseeki64(m_fpOutFile[i], 124380, SEEK_SET);
		fwrite(byteTimelineTrack + i, 1,1, m_fpOutFile[i]);
	}
	return true;
}
bool CCompileAudioMXF::CompileRealDuration()
{
	BYTE byteFooterPosition[8][8];
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		RandNumberConvByte(m_ui64FooterPosition[i], byteFooterPosition[i]);
		//FooterPosition in header
		_fseeki64(m_fpOutFile[i], 49, SEEK_SET);
		fwrite(byteFooterPosition[i], 1, 8, m_fpOutFile[i]);
		//FooterPosition in Bodyer
		_fseeki64(m_fpOutFile[i], 262225, SEEK_SET);//40051
		fwrite(byteFooterPosition[i], 1, 8, m_fpOutFile[i]);
		//FooterPosition in Footer
		_fseeki64(m_fpOutFile[i], m_ui64FooterPosition[i]+33, SEEK_SET);//
		fwrite(byteFooterPosition[i], 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], m_ui64FooterPosition[i] + 49, SEEK_SET);
		fwrite(byteFooterPosition[i], 1, 8, m_fpOutFile[i]);
		//FooterPosition in RIP
		_fseeki64(m_fpOutFile[i], -12, SEEK_END);
		fwrite(byteFooterPosition[i], 1, 8, m_fpOutFile[i]);
		
		_fseeki64(m_fpOutFile[i], 0, SEEK_END);
	}

	return true;
}
bool CCompileAudioMXF::CompileFrameDuration()
{
	BYTE byteFrameDuration[8];
	
	RandNumberConvByte(m_ui64FrameCount*m_ui64AudioLen, byteFrameDuration);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i],266232,SEEK_SET);
		fwrite(byteFrameDuration, 1, 8, m_fpOutFile[i]);
	}
	return true;
}
bool CCompileAudioMXF::Flush()
{
	//Flush Audio Data
	if (m_ui64AudioUseSize != 0)
	{
		for (int i = 0; i < m_ui64AudioChannel; i++)
		{
			fwrite(m_chAudioMixMemory, 1, m_ui64AudioUseSize, m_fpOutFile[i]);
		}

		m_ui64AudioUseSize = 0;
	}
	//AudioDuration
	CompileFrameDuration();
	//Footer
	CompileFooter();
	//Postition
	CompilePosition();
	//RealDuration
	CompileRealDuration();
	//PackageUID
	CompilePackageUID();
	//
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		SAFE_CLOSE(m_fpOutFile[i]);
	}
	return true;
}