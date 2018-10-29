#pragma once
#include "SBS.h"
#ifndef SBSPACKET
#define SBSPACKET
#define MAX_HASH_LEN 128+1
//#define MAX_DATA_LEN 4095+1
#define MAX_DATA_LEN 2700
#define OLD_MAX_DATA_LEN 8190+1
#define MAX_NAME_LEN 100
//#define MAX_PACKET_LEN 9352
#define MAX_PACKET_LEN 3000
#define DEF_MAX_PACKET_LEN 9400
#define MIN_PACKET_LEN 40
class CSBS_Packet
{
public:
#pragma pack(push,1)
	struct Header {
		BYTE Type : 7; // 128개 패킷타입
		BYTE Direction : 1; // 2개, 0: 요청 1:응답 
		BYTE Format : 4; // 파일 지원 확장자 최대 16개 (0은 빈값)
		BYTE Seq : 8;// 패킷 분할되는거 최대 256개
		WORD SLen : 11; // 4095 -> 분할된 파일 최고 크기 4KB(최대 3.9KB까지가 좋을
		UINT FLen : 20; //1048575 -> 파일 최대 크기 1MB(최대 1023KB까지가 좋을듯)
	};
#pragma pack(pop)

#pragma pack(push,1)
	struct Packet {	
		BYTE Type : 7; // 128개 패킷타입
		BYTE Direction : 1; // 2개, 0: 요청 1:응답 
		BYTE Format; // 파일 지원 확장자 최대 16개 (0은 빈값)		
		BYTE Seq; // 패킷 분할되는거 최대 256개
		UINT SLen:12; // 4095 -> 분할된 파일 최고 크기 4KB(최대 3.9KB까지가 좋을
		UINT FLen:20; //1048575 -> 파일 최대 크기 1MB(최대 1023KB까지가 좋을듯)
		
		char Name[MAX_NAME_LEN];// // 이름최대길이 99
		char Hash[MAX_HASH_LEN];		//char Hash[MAX_HASH_LEN]; //129 -> 최대 128
		char EncData[MAX_DATA_LEN];	//char EncData[MAX_DATA_LEN]; // SLen만큼 들어가니까  -> 최대 8KB(최대 7.9KB까지가 좋을듯)		
	};
#pragma pack(pop)

#pragma pack(push,1)
	struct Packet2 {
		Header head;
		char Name[MAX_NAME_LEN];// // 이름최대길이 99
		char Hash[MAX_HASH_LEN];		//char Hash[MAX_HASH_LEN]; //129 -> 최대 128
		char EncData[MAX_DATA_LEN];	//char EncData[MAX_DATA_LEN]; // SLen만큼 들어가니까  -> 최대 8KB(최대 7.9KB까지가 좋을듯)
	};
#pragma pack(pop)

	//패킷 생성: 패킷타입, 방향, 확장자, 순서번호, 파일길이, 분할길이, 파일이름, 암호회된 데이터, 해시값
	struct Packet* CreatePacket(IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char* name, IN const char* EncData, IN const char* Hash);
	//패킷 삭제
	void DestoryPacket(IN Packet **packet);
	//패킷 내용 출력 - 디버그용
	void PrintPacket(IN Packet* packet);
	//받은 패킷 검증
	bool ValidatePacket(IN Packet* packet);
private:
	//패킷 초기화
	bool InitPacket(OUT Packet **packet, IN size_t hashLen, IN size_t dataLen);
	//패킷 값 쓰기 cValue: Validate 결과 값	
	bool GenPacket(OUT Packet * packet, IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char* name, size_t nlen, IN const char* EncData, size_t dlen, IN const char* Hash, size_t hlen);
	//패킷 유효성 검사
	bool Validate(IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char* name, IN const char* EncData, IN const char* Hash);
};
#endif // !SBSPACKET

