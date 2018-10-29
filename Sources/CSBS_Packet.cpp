#include "../Headers/CSBS_Packet.h"

#ifndef _WIN32
#define ZeroMemory(Destination,Length) memset((Destination),0,(Length))
#endif // _WIN32

bool CSBS_Packet::Validate(IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char* name, IN const char* EncData, IN const char* Hash) {
	__CHECK__ chk;
	bool result = false;
	if (type >= 128 || direction >= 2 || format >= 16 || seq >= 512 || slen >= 8191 || flen >= 1048575 || EncData == NULL || Hash == NULL || name == NULL) // 길이값 검증 및 null검사
		return result;

	switch (type) { // 패킷 타입 별 값 검증
	case 0: // 기상청 파일
		if (direction == 0) { //요청
			if (seq == 0 && flen == 0 && slen == 0 && format != 0 && !chk.isEqual(EncData, "") && !chk.isEqual(Hash, "") && !chk.isEqual(name, ""))  result = true;
		}
		else { // 응답
			if (flen != 0 && format != 0 && !chk.isEqual(EncData, "") && !chk.isEqual(Hash, "") && !chk.isEqual(name, "")) result = true;
		}
		break;
	case 1: // 파일 체크(폴링)

		break;
	case 2: // 무결성 검사
		if (direction == 0) { // 요청
			if (seq == 0 && flen == 0 && slen == 0 && chk.isEqual(EncData, "") && !chk.isEqual(Hash, "") && !chk.isEqual(name, "")) result = true;
		}
		else { // 응답
			if (seq == 0 && flen == 0 && slen == 0 && chk.isEqual(EncData, "") && !chk.isEqual(Hash, "") && !chk.isEqual(name, "")) result = true;
		}
		break;
	case 3: // Beacon 검사
		if (direction == 0) { // 요청
			if (seq == 0 && flen == 0 && slen == 0 && format == 0 && chk.isEqual(EncData, "") && chk.isEqual(Hash, "") && chk.isEqual(name, "")) result = true;
		}
		else { // 응답
			if (seq == 0 && flen == 0 && slen == 0 && format == 0 && chk.isEqual(EncData, "") && chk.isEqual(Hash, "") && chk.isEqual(name, "")) result = true;
		}
		break;
	}
	return result;
}

bool CSBS_Packet::InitPacket(OUT Packet **packet, IN size_t hashLen, IN size_t dataLen) {
	bool result = false;
	/* OLD init
	*packet = (struct Packet*)malloc(sizeof(struct Packet)); //패킷에 메모리 할당
	if (*packet != NULL) {
	ZeroMemory(*packet, sizeof(struct Packet)); //패킷 초기화
	//ZeroMemory((*packet)->Name, MAX_NAME_LEN);
	(*packet)->EncData = (char*)malloc(dataLen + 1); //payload 부분 메모리 할당
	if ((*packet)->EncData != NULL)
	ZeroMemory((*packet)->EncData, hashLen + 1); //payload 부분 메모리 초기화
	//(*packet)->Hash = (char*)malloc(hashLen + 1); //payload 부분 메모리 할당
	if ((*packet)->Hash != NULL) {
	ZeroMemory((*packet)->Hash, hashLen + 1); //payload 부분 메모리 초기화
	result = true;
	}
	}
	*/
	*packet = (struct Packet*)malloc(sizeof(struct Packet)); //패킷에 메모리 할당
	if (*packet != NULL) {
		//ZeroMemory(&((*packet)->head), sizeof(Header));
		//(*packet)->EncData = (char*)malloc(dataLen + 1);
		ZeroMemory((*packet)->Hash,MAX_HASH_LEN);
		ZeroMemory((*packet)->Name, MAX_NAME_LEN);
		ZeroMemory((*packet)->EncData, MAX_DATA_LEN);
		return true;
	}
	return result;
}


bool CSBS_Packet::GenPacket(OUT Packet * packet, IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char * name, size_t nlen, IN const char * EncData, size_t dlen, IN const char * Hash, size_t hlen) {
	if (Validate(type, direction, format, seq, flen, slen, name, EncData, Hash)) {
		/* OLD Header *//*
		packet->head.Type = type;
		packet->head.Direction = direction;
		packet->head.Format = format;
		packet->head.Seq = seq;
		packet->head.SLen = slen;
		packet->head.FLen = flen;*/
		/* Header */
		packet->Type = type;
		packet->Direction = direction;
		packet->Format = format;
		packet->Seq = seq;
		packet->SLen = slen;
		packet->FLen = flen;		
		/* payload */
		memcpy(packet->Name, name, nlen);
		memcpy(packet->Hash, Hash, hlen);
		memcpy(packet->EncData, EncData, dlen);
		/*
		packet->Type = type;
		packet->Direction = direction;
		packet->Format = format;
		packet->Seq = seq;
		packet->SLen = slen;
		packet->FLen = flen;
		*/
		return true;
	}
	return false;
}

CSBS_Packet::Packet* CSBS_Packet::CreatePacket(IN WORD type, IN WORD direction, IN WORD format, IN WORD seq, IN size_t flen, IN WORD slen, IN const char* name, IN const char* EncData, IN const char* Hash) {
	//struct Packet *packet = (struct Packet*)malloc(sizeof(struct Packet));
	struct Packet *packet;
	const size_t nlen = strlen(name), hlen = strlen(Hash), dlen = strlen(EncData);
	if (InitPacket(&packet, hlen, dlen)) {
		if (GenPacket(packet, type, direction, format, seq, flen, slen, name, nlen, EncData, dlen, Hash, hlen))
			return packet;
	}
	return NULL;
}

void CSBS_Packet::DestoryPacket(IN Packet ** packet){
	//free((*packet)->Name);
	if (packet == NULL || *packet == NULL) return;
	//free((*packet)->EncData);
	free(*packet);
}
/*
void CSBS_Packet::PrintPacket(IN Packet * packet){
	if (packet == NULL) return;

	struct Header head;
	const size_t hsize = sizeof(Header);
	const size_t bsize = sizeof(Packet) - sizeof(Header);
	if(packet->EncData!=NULL)	bsize + strlen(packet->EncData);
	
	printf("word: %zd\nBYTE: %zd\nuint: %zd\nheader struct: %zd\n", sizeof(WORD),sizeof(BYTE),sizeof(UINT),sizeof(struct Header));
	printf("head: %zd\n", hsize);
	printf("payload Length: %zd\n", bsize);
	printf("packet: %zd\n", hsize+bsize);
	const char* sep = "=========================================================================\n";
	printf("%s", sep);
	printf("Type: %u\n", packet->head.Type);	
	printf("Dire: %u\n", packet->head.Direction);
	printf("Form: %u\n", packet->head.Format);
	printf("Sequ: %u\n", packet->head.Seq);
	printf("SLen: %u\n", packet->head.SLen);
	printf("FLen: %zu\n", packet->head.FLen);
	if(packet->Name !=NULL )
	printf("Name: %s\n", packet->Name);
	if(packet->Hash !=NULL )
	printf("Hash: %s\n", packet->Hash);
	if(packet->EncData !=NULL )
	printf("EncD: %s\n", packet->EncData);
	printf("%s",sep);
}*/

void CSBS_Packet::PrintPacket(IN Packet * packet){
	if (packet == NULL) return;
	const size_t psize = sizeof(Packet);
	const char* sep = "=========================================================================\n";
	printf("%s", sep);
	printf("Type: %u\n", packet->Type);	
	printf("Dire: %u\n", packet->Direction);
	printf("Form: %u\n", packet->Format);
	printf("Sequ: %u\n", packet->Seq);
	printf("SLen: %u\n", packet->SLen);
	printf("FLen: %zu\n", packet->FLen);
	if(packet->Name !=NULL )
		printf("Name: %s\n", packet->Name);
	if(packet->Hash !=NULL )
		printf("Hash: %s\n", packet->Hash);
	if(packet->EncData !=NULL )
		printf("EncD: %s\n", packet->EncData);
	printf("%s",sep);
}

bool CSBS_Packet::ValidatePacket(IN Packet * packet)
{
	return packet != NULL ? Validate(packet->Type, packet->Direction, packet->Format, packet->Seq, packet->FLen, packet->SLen, packet->Name, packet->EncData, packet->Hash) : false;
}



/*
bool CSBS_Packet::ValidatePacket(IN Packet * packet)
{
	return packet != NULL ? Validate(packet->head.Type, packet->head.Direction, packet->head.Format, packet->head.Seq, packet->head.FLen, packet->head.SLen, packet->Name, packet->EncData, packet->Hash) : false;
}
*/

