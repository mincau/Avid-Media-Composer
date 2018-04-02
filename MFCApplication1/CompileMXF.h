#pragma once
#define SAFE_CLOSE(p) \
if((p))\
{ \
	fclose((p));\
	(p)=nullptr;\
}
#define SAFE_DELETE(p)\
if((p))\
{\
	delete[] (p);\
	(p) = nullptr;\
}
typedef unsigned long long uint64_t;

class CCombinationMXF
{
public:
	CCombinationMXF();
	~CCombinationMXF();
private:
	FILE		*m_fpOutFile[9];
	FILE		*m_fpTemplateFile[2];
	uint64_t	m_ui64FooterSize;

	char		*m_chAudioHeadMemory;
	char		*m_chAudioMixMemory;
	char		*m_chFooterMemory;
	//Audio
	uint64_t	m_ui64AudioHeaderSize;
	uint64_t	m_ui64AudioLen;
	uint64_t	m_ui64AudioFrameCount;
	uint64_t	m_ui64AudioCriSize;//critical
	uint64_t	m_ui64AudioMixSize;
	uint64_t	m_ui64AudioUseSize;
	uint64_t	m_ui64FooterPosition[9];
	uint64_t	m_ui64AudioChannel;
	//Video
	uint64_t	m_ui64VideoHeaderSize;
	uint64_t	m_ui64VideoLen;
	uint64_t	m_ui64VideoFrameCount;
	uint64_t	m_ui64VideoMixSize;
	uint64_t	m_ui64VideoCriSize;
	uint64_t	m_ui64VideoUseSize;
	char		*m_chVideoHeadMemory;
	char		*m_chVidoMixMemory;
	//
	BYTE		m_byteTrack[8][8];
	BYTE		m_bytePackID[8];
	BYTE		m_byteSourceID[8];
	BYTE		m_byteCurrentTime[8];

	bool CombinationAudioFooter();
	void GetRandomNumber();
	void RandNumberConvByte(uint64_t in_ui64Number,BYTE *in_byte);
	bool ModifyAudioFilesCurrentTime();
	bool ModifyAudioOccupationBytes();
	bool ModifyAudioContainerData();
	bool ModifyAudioPackageUID();
	bool ModifyAudioFooterPosition();

	bool ModifyVideoIndexTabFrameDuration();
	bool ModifyVideoPackageUID();
	bool ModifyVideoFooterPosition();
	bool ModifyVideoContainerData();
	bool CombinationVideoFooter();
	bool ModifyVideoFileCurrentTime();
	bool ModifyVideoOccupationBytes();
	bool CombinationAudioHeader();
	bool CombinationVideoHeader();
	bool FlushAudio();
	bool FlushVideo();
public:
	bool SetRWMXFPath(char *in_chReadPath,char *in_chOutPath);
	bool CombinationHeader();
	bool CombinationAudioData(char *in_chAudio,uint64_t in_ui64Len);
	bool CombinationVideoData(char *in_chAudio, uint64_t in_ui64Len);
	bool Flush();
};

