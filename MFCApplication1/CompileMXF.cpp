#include "stdafx.h"
#include "CompileMXF.h"
#include <conio.h>

CCombinationMXF::CCombinationMXF()
{
	m_fpTemplateFile[0] = nullptr;//Audio
	m_fpTemplateFile[1] = nullptr;//Video
	m_chAudioHeadMemory = nullptr;
	m_chAudioMixMemory = nullptr;
	m_chVideoHeadMemory = nullptr;
	m_chVidoMixMemory = nullptr;

	m_ui64FooterSize = 577;
	//Video
	m_ui64VideoHeaderSize = 393191 + 25;
	m_chVideoHeadMemory = new char[m_ui64VideoHeaderSize];
	m_ui64VideoLen = 3887104;
	m_ui64VideoMixSize = m_ui64VideoLen * 7;
	m_ui64VideoCriSize = m_ui64VideoLen * 5;
	m_chVidoMixMemory = new char[m_ui64VideoMixSize];
	m_ui64VideoUseSize = 0;
	m_ui64VideoFrameCount = 0;
	//Audio
	m_ui64AudioHeaderSize = 266215 + 25;
	m_ui64AudioLen = 1920;
	m_ui64AudioMixSize = m_ui64AudioLen * 7;
	m_ui64AudioCriSize = m_ui64AudioLen * 5;
	m_ui64AudioUseSize = 0;
	m_chAudioMixMemory = new char[m_ui64AudioMixSize];
	m_chAudioHeadMemory = new char[m_ui64AudioHeaderSize];

	m_chFooterMemory = new char[m_ui64FooterSize];

	m_ui64AudioChannel = 8;
	m_ui64AudioFrameCount = 0;

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		memset(m_byteTrack[i], 0x00, 8);
		m_fpOutFile[i] = nullptr;
	}
	memset(m_byteCurrentTime, 0x00, 8);

}

CCombinationMXF::~CCombinationMXF()
{
	//Create Files
	for (int i = 0; i < m_ui64AudioChannel;i++)
	{
		SAFE_CLOSE(m_fpOutFile[i]);
	}
	SAFE_CLOSE(m_fpOutFile[8]);
	//Template Files
	for (int i = 0; i < 2; i++)
	{
		SAFE_CLOSE(m_fpTemplateFile[i]);
	}
	//Video
	
	SAFE_DELETE(m_chVideoHeadMemory);
	SAFE_DELETE(m_chVidoMixMemory);
	//Audio
	SAFE_DELETE(m_chAudioHeadMemory);
	SAFE_DELETE(m_chAudioMixMemory);
	SAFE_DELETE(m_chFooterMemory);

}
void CCombinationMXF::RandNumberConvByte(uint64_t in_ui64Number, BYTE *in_byte)
{
	for (int i = 7; i >= 0;i--)
	{
		in_byte[i] = (BYTE)(in_ui64Number & 0x00000000000000ff);
		in_ui64Number >>= 8;
	}
}
void CCombinationMXF::GetRandomNumber()
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
bool CCombinationMXF::SetRWMXFPath(char *in_chReadPath, char *in_chOutPath)
{
	errno_t er;

	m_fpTemplateFile[0] = _fsopen("C:\\DNxHR_Audio.mxf", "rb+", SH_DENYNO);
	if (nullptr == m_fpTemplateFile[0])
	{
		_cprintf("_fsopen DNxHR_Audio faile\n");
		return false;
	}
	
	m_fpTemplateFile[1] = _fsopen("C:\\DNxHR_Video.mxf", "rb+", SH_DENYNO);
	if (nullptr == m_fpTemplateFile[1])
	{
		_cprintf("_fsopen DNxHR_Video faile\n");
		return false;
	}
	
	GetRandomNumber();
	
	char chPath[MAX_PATH] = {0};
	//Audio
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
	//Video
	sprintf_s(chPath, "%s\\%0x%0x%0x%0x%0x%0x%0x%0x%s", in_chOutPath, m_byteSourceID[0],
		m_byteSourceID[1], m_byteSourceID[2], m_byteSourceID[3], m_byteSourceID[4], m_byteSourceID[5],
		m_byteSourceID[6], m_byteSourceID[7], ".mxf");
	
	er = fopen_s(&m_fpOutFile[8], chPath, "wb+");
	if (er != 0)
	{
		_cprintf("fopen_s OutPath %s faile\n", chPath);
		return false;
	}

	memset(chPath, 0, MAX_PATH);

	return true;
}
bool CCombinationMXF::CombinationVideoHeader()
{
	fread_s(m_chVideoHeadMemory,m_ui64VideoHeaderSize,1,m_ui64VideoHeaderSize,m_fpTemplateFile[1]);

	fwrite(m_chVideoHeadMemory,1,m_ui64VideoHeaderSize,m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::CombinationAudioHeader()
{
	fread_s(m_chAudioHeadMemory, m_ui64AudioHeaderSize, 1, m_ui64AudioHeaderSize, m_fpTemplateFile[0]);

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		fwrite(m_chAudioHeadMemory, 1, m_ui64AudioHeaderSize, m_fpOutFile[i]);
	}
	return true;
}
bool CCombinationMXF::CombinationHeader()
{
	//Audio Header
	CombinationAudioHeader();
	//Video Header
	CombinationVideoHeader();

	return true;
}
bool CCombinationMXF::CombinationVideoData(char *in_chVideo, uint64_t in_ui64Len)
{
	if (in_ui64Len != m_ui64VideoLen)
	{
		_cprintf("CombinationVideoData faile\n");
		return false;
	}
	
	m_ui64VideoFrameCount++;

	memcpy_s(m_chVidoMixMemory + m_ui64VideoUseSize,m_ui64VideoMixSize,in_chVideo,in_ui64Len);
	m_ui64VideoUseSize += in_ui64Len;

	if (m_ui64VideoUseSize > m_ui64VideoCriSize)
	{
		fwrite(m_chVidoMixMemory, 1, m_ui64VideoUseSize, m_fpOutFile[8]);
		m_ui64VideoUseSize = 0;
	}
	return true;
}
bool CCombinationMXF::CombinationAudioData(char *in_chAudio, uint64_t in_ui64Len)
{
	if (in_ui64Len != m_ui64AudioLen)
	{
		printf("CombinationAudioData faile\n");
		return false;
	}
	m_ui64AudioFrameCount++;

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
bool CCombinationMXF::CombinationVideoFooter()
{
	_fseeki64(m_fpOutFile[8],0,SEEK_END);

	_fseeki64(m_fpTemplateFile[1], -577, SEEK_END);

	m_ui64FooterPosition[8] = _ftelli64(m_fpOutFile[8]);

	fread_s(m_chFooterMemory, m_ui64FooterSize, 1, m_ui64FooterSize, m_fpTemplateFile[1]);
	
	fwrite(m_chFooterMemory, 1, m_ui64FooterSize, m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::CombinationAudioFooter()
{
	_fseeki64(m_fpTemplateFile[0], 725024, SEEK_SET);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i], 0, SEEK_END);
		m_ui64FooterPosition[i] = _ftelli64(m_fpOutFile[i]);
	}
	fread_s(m_chFooterMemory, m_ui64FooterSize, 1, m_ui64FooterSize, m_fpTemplateFile[0]);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i], 0, SEEK_END);
		fwrite(m_chFooterMemory, 1, m_ui64FooterSize, m_fpOutFile[i]);
	}

	return true;
}
bool CCombinationMXF::ModifyVideoContainerData()
{
	BYTE byteFrameCount[8];
	BYTE byteDuration[8];

	RandNumberConvByte(m_ui64VideoFrameCount, byteFrameCount);
	RandNumberConvByte(m_ui64VideoFrameCount * 960, byteDuration);

	_fseeki64(m_fpOutFile[8], 120202, SEEK_SET);//Sequenc 1D58A
	fwrite(byteFrameCount, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 120335, SEEK_SET);//Source Clip 1D60F
	fwrite(byteFrameCount, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 120551, SEEK_SET);//Sequenc	1D6e7
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 120684, SEEK_SET);//Source Clips	1D76C
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 120900, SEEK_SET);//Sequenc		1D844
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 121033, SEEK_SET);//Source Clips	1D8c9
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 121249, SEEK_SET);//Sequenc		1D9a1
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 121382, SEEK_SET);//Source Clips	1Da26
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 121598, SEEK_SET);//Sequenc		1Dafe
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 121731, SEEK_SET);//Source Clips	1Db83
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 121947, SEEK_SET);//Sequenc		1Dc5b
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 122080, SEEK_SET);//Source Clips	1Dce0
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 122296, SEEK_SET);//Sequenc		1ddb8
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 122429, SEEK_SET);//Source Clips	1de3d
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 122645, SEEK_SET);//Sequenc		1df15
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 122778, SEEK_SET);//Source Clips	1df9a
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 122994, SEEK_SET);//Sequenc		1e072
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 123127, SEEK_SET);//Source Clips	1e0f7
	fwrite(byteDuration, 1, 8, m_fpOutFile[8]);
	
	_fseeki64(m_fpOutFile[8], 123911, SEEK_SET);//CDCI	1E407
	fwrite(byteFrameCount, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 124119, SEEK_SET);//Sequenc		1e4d7
	fwrite(byteFrameCount, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 124252, SEEK_SET);//Source Clips	1e55c
	fwrite(byteFrameCount, 1, 8, m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::ModifyAudioContainerData()
{
	BYTE byteFrameCount[8];
	BYTE byteDuration[8];

	RandNumberConvByte(m_ui64AudioFrameCount, byteFrameCount);
	RandNumberConvByte(m_ui64AudioFrameCount*960, byteDuration);

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
bool CCombinationMXF::ModifyVideoIndexTabFrameDuration()
{
	BYTE byteFrameCount[8];
	
	RandNumberConvByte(m_ui64VideoFrameCount, byteFrameCount);

	_fseeki64(m_fpOutFile[8],-305,SEEK_END);
	fwrite(byteFrameCount,1,8,m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::ModifyVideoPackageUID()
{
	BYTE byteTimelineTrack[] = {0x02};
	//Material Package
	_fseeki64(m_fpOutFile[8], 88349, SEEK_SET);//1591d
	fwrite(m_bytePackID, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 118794, SEEK_SET);//1d00a
	fwrite(m_bytePackID, 1, 8, m_fpOutFile[8]);
	//Source Package 
	_fseeki64(m_fpOutFile[8], 88397, SEEK_SET);//1594d
	fwrite(m_byteSourceID, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 118470, SEEK_SET);//1cec6
	fwrite(m_byteSourceID, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], 123336, SEEK_SET);//1e1c8
	fwrite(m_byteSourceID, 1, 8, m_fpOutFile[8]);
	//SourceClip
	_fseeki64(m_fpOutFile[8], 120315, SEEK_SET);//1d5fb
	fwrite(m_byteSourceID, 1, 8, m_fpOutFile[8]);
	//SourceClip	A01
	_fseeki64(m_fpOutFile[8], 120663, SEEK_SET);
	fwrite(m_byteTrack[0], 1, 8, m_fpOutFile[8]);
	//SourceClip	A02
	_fseeki64(m_fpOutFile[8], 121013, SEEK_SET);
	fwrite(m_byteTrack[1], 1, 8, m_fpOutFile[8]);
	//SourceClip	A03
	_fseeki64(m_fpOutFile[8], 121362, SEEK_SET);
	fwrite(m_byteTrack[2], 1, 8, m_fpOutFile[8]);
	//SourceClip	A04
	_fseeki64(m_fpOutFile[8], 121711, SEEK_SET);
	fwrite(m_byteTrack[3], 1, 8, m_fpOutFile[8]);
	//SourceClip	A05
	_fseeki64(m_fpOutFile[8], 122060, SEEK_SET);
	fwrite(m_byteTrack[4], 1, 8, m_fpOutFile[8]);
	//SourceClip	A06
	_fseeki64(m_fpOutFile[8], 122409, SEEK_SET);
	fwrite(m_byteTrack[5], 1, 8, m_fpOutFile[8]);
	//SourceClip	A07
	_fseeki64(m_fpOutFile[8], 122758, SEEK_SET);
	fwrite(m_byteTrack[6], 1, 8, m_fpOutFile[8]);
	//SourceClip	A08
	_fseeki64(m_fpOutFile[8], 123107, SEEK_SET);
	fwrite(m_byteTrack[7], 1, 8, m_fpOutFile[8]);
	//Timeline Track
	//SourceClip SourceTrackID
	return true;
}
bool CCombinationMXF::ModifyAudioPackageUID()
{
	BYTE byteTimelineTrack[] = { 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B };
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		//Material Package
		_fseeki64(m_fpOutFile[i], 88377, SEEK_SET);
		fwrite(m_bytePackID, 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 118822, SEEK_SET);
		fwrite(m_bytePackID, 1, 8, m_fpOutFile[i]);
		//Source Package 
		_fseeki64(m_fpOutFile[i], 88425, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 118498, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 8, m_fpOutFile[i]);
		_fseeki64(m_fpOutFile[i], 123380, SEEK_SET);
		fwrite(m_byteTrack[i], 1, 8, m_fpOutFile[i]);
		//SourceClip
		_fseeki64(m_fpOutFile[i], 120343, SEEK_SET);
		fwrite(m_byteSourceID, 1, 8, m_fpOutFile[i]);
		//SourceClip	A01
		_fseeki64(m_fpOutFile[i], 120692, SEEK_SET);
		fwrite(m_byteTrack[0], 1, 8, m_fpOutFile[i]);
		//SourceClip	A02
		_fseeki64(m_fpOutFile[i], 121041, SEEK_SET);
		fwrite(m_byteTrack[1], 1, 8, m_fpOutFile[i]);
		//SourceClip	A03
		_fseeki64(m_fpOutFile[i], 121390, SEEK_SET);
		fwrite(m_byteTrack[2], 1, 8, m_fpOutFile[i]);
		//SourceClip	A04
		_fseeki64(m_fpOutFile[i], 121739, SEEK_SET);
		fwrite(m_byteTrack[3], 1, 8, m_fpOutFile[i]);
		//SourceClip	A05
		_fseeki64(m_fpOutFile[i], 122088, SEEK_SET);
		fwrite(m_byteTrack[4], 1, 8, m_fpOutFile[i]);
		//SourceClip	A06
		_fseeki64(m_fpOutFile[i], 122437, SEEK_SET);
		fwrite(m_byteTrack[5], 1, 8, m_fpOutFile[i]);
		//SourceClip	A07
		_fseeki64(m_fpOutFile[i], 122786, SEEK_SET);
		fwrite(m_byteTrack[6], 1, 8, m_fpOutFile[i]);
		//SourceClip	A08
		_fseeki64(m_fpOutFile[i], 123135, SEEK_SET);
		fwrite(m_byteTrack[7], 1, 8, m_fpOutFile[i]);
		//Timeline Track
		_fseeki64(m_fpOutFile[i], 124204, SEEK_SET);
		fwrite(byteTimelineTrack+i, 1,1, m_fpOutFile[i]);
		//SourceClip SourceTrackID
		_fseeki64(m_fpOutFile[i], 124380, SEEK_SET);
		fwrite(byteTimelineTrack + i, 1,1, m_fpOutFile[i]);
	}
	return true;
}
bool CCombinationMXF::ModifyVideoFooterPosition()
{
	BYTE byteFooterPosition[8] = {0x00};

	RandNumberConvByte(m_ui64FooterPosition[8], byteFooterPosition);

	//FooterPosition in header
	_fseeki64(m_fpOutFile[8], 49, SEEK_SET);
	fwrite(byteFooterPosition, 1, 8, m_fpOutFile[8]);
	//FooterPosition in Bodyer
	_fseeki64(m_fpOutFile[8], 262225, SEEK_SET);//40051
	fwrite(byteFooterPosition, 1, 8, m_fpOutFile[8]);
	//FooterPosition in Footer
	_fseeki64(m_fpOutFile[8], m_ui64FooterPosition[8] + 33, SEEK_SET);//
	fwrite(byteFooterPosition, 1, 8, m_fpOutFile[8]);
	_fseeki64(m_fpOutFile[8], m_ui64FooterPosition[8] + 49, SEEK_SET);
	fwrite(byteFooterPosition, 1, 8, m_fpOutFile[8]);
	//FooterPosition in RIP
	_fseeki64(m_fpOutFile[8], -12, SEEK_END);
	fwrite(byteFooterPosition, 1, 8, m_fpOutFile[8]);

	_fseeki64(m_fpOutFile[8], 0, SEEK_END);

	return true;
}
bool CCombinationMXF::ModifyAudioFooterPosition()
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
bool CCombinationMXF::ModifyVideoFileCurrentTime()
{
	_fseeki64(m_fpOutFile[8],88573,SEEK_SET);
	fwrite(m_byteCurrentTime, 1, 8, m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::ModifyAudioFilesCurrentTime()
{
	SYSTEMTIME sys;
	WORD wYear;
	WORD wMonth;
	WORD wDay;
	WORD wHour;
	WORD wMinute;
	WORD wSecond;

	GetLocalTime(&sys);

	wYear = sys.wYear;
	wMonth = sys.wMonth;
	wDay = sys.wDay;
	wHour = sys.wHour;
	wMinute = sys.wMinute;
	wSecond = sys.wSecond;

	for (int i = 1; i >= 0; i--)
	{
		m_byteCurrentTime[i] = (BYTE)(wYear & 0x00ff);
		wYear >>= 8;
	}
	m_byteCurrentTime[2] = (BYTE)(wMonth & 0xff);
	m_byteCurrentTime[3] = (BYTE)(wDay & 0xff);
	m_byteCurrentTime[4] = (BYTE)(wHour & 0xff);
	m_byteCurrentTime[5] = (BYTE)(wMinute & 0xff);
	m_byteCurrentTime[6] = (BYTE)(wSecond & 0xff);

	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i],88601,SEEK_SET);//15A19
		fwrite(m_byteCurrentTime,1,8, m_fpOutFile[i]);
		//_fseeki64(m_fpOutFile[i], 118752, SEEK_SET);//1CFE0
		//fwrite(byteCurrentTime, 1, 8, m_fpOutFile[i]);
		//_fseeki64(m_fpOutFile[i], 123324, SEEK_SET);//1e1bc
		//fwrite(byteCurrentTime, 1, 8, m_fpOutFile[i]);
	}
	return true;
}
bool CCombinationMXF::ModifyVideoOccupationBytes()
{
	BYTE byteFrameDuration[8];
	
	RandNumberConvByte(m_ui64VideoFrameCount*m_ui64VideoLen, byteFrameDuration);

	_fseeki64(m_fpOutFile[8],393191+17,SEEK_SET);
	fwrite(byteFrameDuration, 1, 8, m_fpOutFile[8]);

	return true;
}
bool CCombinationMXF::ModifyAudioOccupationBytes()
{
	BYTE byteFrameDuration[8];
	
	RandNumberConvByte(m_ui64AudioFrameCount*m_ui64AudioLen, byteFrameDuration);
	
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		_fseeki64(m_fpOutFile[i],266232,SEEK_SET);
		fwrite(byteFrameDuration, 1, 8, m_fpOutFile[i]);
	}
	return true;
}
bool CCombinationMXF::FlushVideo()
{
	//data
	if (m_ui64VideoUseSize != 0)
	{
		fwrite(m_chVidoMixMemory,1,m_ui64VideoUseSize,m_fpOutFile[8]);
		m_ui64VideoUseSize = 0;
	}
	//CurrentTime
	ModifyVideoFileCurrentTime();
	//Video Bytes Length
	ModifyVideoOccupationBytes();
	//Footer
	CombinationVideoFooter();
	//Modify Frame Count in Container Data
	ModifyVideoContainerData();
	//Footer Position
	ModifyVideoFooterPosition();
	//PackageUID
	ModifyVideoPackageUID();
	//IndexTabSegment
	ModifyVideoIndexTabFrameDuration();
	//
	SAFE_CLOSE(m_fpOutFile[8]);
	
	return true;
}
bool CCombinationMXF::FlushAudio()
{
	if (m_ui64AudioUseSize != 0)
	{
		for (int i = 0; i < m_ui64AudioChannel; i++)
		{
			fwrite(m_chAudioMixMemory, 1, m_ui64AudioUseSize, m_fpOutFile[i]);
		}

		m_ui64AudioUseSize = 0;
	}
	//CurrentTime
	ModifyAudioFilesCurrentTime();
	//Audio Bytes Length
	ModifyAudioOccupationBytes();
	//Footer
	CombinationAudioFooter();
	//Modify Frame Count in Container Data
	ModifyAudioContainerData();
	//Footer Position
	ModifyAudioFooterPosition();
	//PackageUID
	ModifyAudioPackageUID();
	//
	for (int i = 0; i < m_ui64AudioChannel; i++)
	{
		SAFE_CLOSE(m_fpOutFile[i]);
	}
	return true;
}
bool CCombinationMXF::Flush()
{
	//Flush Audio Data
	FlushAudio();
	//Flush Video Data
	FlushVideo();

	return true;
}