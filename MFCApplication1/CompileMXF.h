#pragma once
#define SAFE_CLOSE(p) \
if((p))\
{ \
	fclose((p));\
	(p)=nullptr;\
}
typedef unsigned long long uint64_t;
class CCompileAudioMXF
{
public:
	CCompileAudioMXF();
	~CCompileAudioMXF();
private:
	FILE		*m_fpOutFile[8];
	FILE		*m_fpReadFile;
	FILE		*m_fpTemplateFile;

	char		*m_chHeadMemory;
	char		*m_chAudioMixMemory;
	char		*m_chFooterMemory;

	uint64_t	m_ui64HeaderSize;
	uint64_t	m_ui64AudioLen;
	uint64_t	m_ui64FrameCount;
	uint64_t	m_ui64AudioCriSize;//critical
	uint64_t	m_ui64AudioMixSize;
	uint64_t	m_ui64AudioUseSize;
	uint64_t	m_ui64FooterPosition[8];
	uint64_t	m_ui64AudioChannel;

	BYTE		m_byteTrack[8][8];
	BYTE		m_bytePackID[8];
	BYTE		m_byteSourceID[8];

	void GetRandomNumber();
	void RandNumberConvByte(uint64_t in_ui64Number,BYTE *in_byte);
	bool CompileFrameDuration();
	bool CompileFooter();
	bool CompilePosition();
	bool CompilePackageUID();
	bool CompileRealDuration();
public:
	bool SetRWMXFPath(char *in_chReadPath,char *in_chOutPath);
	bool CompileHeader();
	bool CompileAudioData(char *in_chAudio,uint64_t in_ui64Len);
	bool Flush();
};

