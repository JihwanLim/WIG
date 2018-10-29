#include "../Headers/crypto.h"
#include <iostream>
using namespace std;
/* 암복호화 전 값 체크할때 쓰는 클래스 (공통적으로 사용) */


bool __CHECK__::isNull(IN void* value) {
	return value == NULL;
}

bool __CHECK__::isEqual(IN const char* letter, IN const char* value) {
	if (isNull((char*)value) || isNull((char*)letter))
		return false;
#ifdef _WIN32
	return _stricmp(letter, value) == 0;
#else
	return strcasecmp(letter, value) == 0;
#endif
}

bool __CHECK__::validate(IN const char* algo, IN const char*mode, IN BYTE* key, IN BYTE * iv, IN BYTE* in) {
	__CHECK__ check;
	if (!(isNull((char*)algo) || isNull(key) || isNull(iv) || isNull(in))) {
		if (check.isEqual("seed", algo) || check.isEqual("aes", algo))
			return !isNull((char*)mode);
		else if (check.isEqual("aria", algo))
			return true;
	}
	return false;
}


/* 암복호화, 해시, 파일 읽기/쓰기에 사용 Encrypt/Decrypt/Hash Library */

int __EDH__::hash(IN char * in, IN const char * algo, OUT char** output) {
	__OPENSSL__ op;
	int md_len;
	const EVP_MD* md = op.getMd(algo, &md_len);
	(*output) = (char*)OPENSSL_malloc(md_len * 2 + 1);
	if (md != NULL) {
		BYTE* hout;
		if (op.hash(in, &hout, md, md_len))
			for (int i = 0; i < md_len; i++) {
				snprintf((char*)&(*output)[i * 2],(md_len*2), "%02x", (UINT)hout[i]);
			}
		else
			snprintf(*output, (md_len * 2), "fail");
		OPENSSL_free(hout);
	}
	else snprintf(*output, (md_len * 2), "noAlgo");
	return md_len;
}

bool __EDH__::readFile(IN char * FileName, OUT BYTE ** save, OUT PSIZE_T length)
{
	FILE* in;
	if (openFile(&in, FileName, "rb")) {
		fseek(in, 0L, SEEK_END);
		size_t size = ftell(in);
		fseek(in, 0L, SEEK_SET);
		if (in != NULL) {
			*save = (BYTE*)OPENSSL_malloc(size + 1);		//*save = (char*)OPENSSL_malloc(size + 1);
			if (fread(*save, sizeof(char), size, in) != size)	return false;
			*length = (int)size;
			fclose(in);
			return true;
		}
		else {
			*save = NULL;
		}
	}
	return false;
}


bool __EDH__::writeFile(IN char * FileName, IN char * data, IN size_t data_len)
{
	FILE* out;	
	if (openFile(&out, FileName, "wb")) {
		//= fopen(FileName, "wb");
		if (out != NULL) {
			if (fwrite(data, sizeof(char), data_len, out) != data_len) return false;
			fclose(out);
			//OPENSSL_free(data);
			return true;
		}
	}
	return false;
}

bool __EDH__::openFile(OUT FILE** fp, IN char* FileName, IN const char* mode)
{
#ifdef _WIN32
	fopen_s(fp, FileName, mode);
#else
	*fp = fopen(FileName, mode);
#endif
	return (*fp != NULL);
}

bool __EDH__::encrypt(IN const char* algo, IN const char* mode, IN BYTE* key, IN BYTE* iv, IN BYTE* in, IN size_t inLen, OUT char** out, OUT PSIZE_T outLen) {
	__CHECK__ check;
	bool result = false;
	if (check.validate(algo, mode, key, iv, in)) {
		__OPENSSL__ op;
		__KISA__ ki;
		BYTE* output = NULL;
		size_t opLen_d = 0;
		PSIZE_T opLen = (PSIZE_T)&opLen_d;
		if (check.isEqual("seed", algo) || check.isEqual("aes", algo)) {
			output = (BYTE*)OPENSSL_malloc(inLen * 2 + 1);
			const EVP_CIPHER* cipher = op.getCipher(algo, mode);
			int new_inLen = inLen;

			if (cipher != NULL) {
				result = op.encrypt(key, iv, in, new_inLen, output, opLen, cipher);
			}
			else {
				result = false;
			}

		}
		else if (check.isEqual("aria", algo)) {
			const size_t count = (inLen % 16) == 0 ? inLen / 16 : inLen / 16 + 1;
			output = (BYTE*)OPENSSL_malloc(sizeof(BYTE*)*count + 1);//OPENSSL_malloc((count * 16) + 1);
			result = ki.encrypt(in, key, iv, inLen, opLen, output);
		}
		else {
			// openssl, kisa library를 제외한 다른 라이브러리 추가시 여기에 추가하면됨
			output = (BYTE*)OPENSSL_malloc(1);
		}


		if (result) {
			//암호화 성공시 base64 인코딩을함
			op.Base64E(output, (int)*opLen, out, outLen);
		}
		OPENSSL_free(output);
		//OPENSSL_free(in);
	}
	return result;
}

bool __EDH__::decrypt(IN const char* algo, IN const char* mode, IN BYTE* key, IN BYTE* iv, IN BYTE* in, IN size_t inLen, OUT char** out, OUT PSIZE_T outLen) {
	__CHECK__ check;
	bool result = false;
	if (check.validate(algo, mode, key, iv, (BYTE*)in)) {
		__OPENSSL__ op;
		__KISA__ ki;
		int inputLen;
		BYTE* input = NULL;
		op.Base64D(in, inLen, &input, &inputLen);
		BYTE* output=NULL;//[(inputLen * 2) + 1]();
		size_t opLen_d = 0;
		PSIZE_T opLen = (PSIZE_T)&opLen_d;
		if (check.isEqual("seed", algo) || check.isEqual("aes", algo)) {
			output = (BYTE*)OPENSSL_malloc(inputLen * 2 + 1);
			const EVP_CIPHER* cipher = op.getCipher(algo, mode);
			if (cipher != NULL) {
				result = op.decrypt(key, iv, input, inputLen, output, opLen, cipher);
			}
			else {
				result = false;
			}
		}
		else if (check.isEqual("aria", algo)) {
			//(BYTE*)OPENSSL_malloc(inputLen * 2+1);//[(inputLen * 2) + 1]();
			const size_t count = (inputLen % 16) == 0 ? inputLen / 16 : inputLen / 16 + 1;
			//output = (BYTE*)OPENSSL_malloc(count * 2 + 1);
			result = ki.decrypt(input, key, iv, inputLen, opLen, &output);
			/*
			//여기는 따로 짜야될듯 암호화 돌리는거 사이즈 동적어쩌고 하는거로 아마

			BYTE rk[16 * 17], mk[32];// c[16],
									 //BYTE p[16] = { 0x11, 0x11, 0x11, 0x11, 0xaa, 0xaa, 0xaa, 0xaa, 0x11, 0x11, 0x11, 0x11, 0xbb, 0xbb, 0xbb, 0xbb };
			for (int i = 0; i < 16; i++)
				mk[i] = i * 0x11;
			for (int i = 16; i < 24; i++)
				mk[i] = (i - 16) * 0x11;
			//mk가 변경되어야 하는데 어떻게 자동생성하지...
			BYTE *buffer = (BYTE*)OPENSSL_malloc(16);//new BYTE[16]();
			memcpy(buffer, input, inLen);
			size_t paddingSize = 16 - inLen;
			size_t start = 16 - paddingSize;
			ki.delPadding(buffer, (int)start, 16);
			ki.Crypt(buffer, ki.EncKeySetup(mk, rk, 256), rk, output);
			//printf("[result]\n");  ki.printBlock(c); printf("\n");
			//ki.Crypt(c, ki.DecKeySetup(mk, rk, 256), rk, out);
			opLen = 16;
			//		printf("[result]\n"); ki.printBlock(out); printf("\n");
			OPENSSL_free(buffer);
			result = true;
			*/
		}
		else {
			// openssl, kisa library를 제외한 다른 라이브러리 추가시 여기에 추가하면됨
		}
		if (result) {
			//
			(*out) = (char*)OPENSSL_malloc(*opLen + 1);
			memcpy(*out, output, *opLen);
			*outLen = *opLen;
			OPENSSL_free(output);
		}
		OPENSSL_free(input);
		OPENSSL_free(in);
	}
	return result;
}


/* OPENSSL Library */

bool __OPENSSL__::encrypt(IN BYTE* key, IN BYTE* iv, IN BYTE* in, IN int inLen, OUT BYTE* out, OUT PSIZE_T outLen, IN const EVP_CIPHER * algo)
{
	int encLen;//< 암호화된 길이 저장 변수
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();//<사이퍼 컨텍스트 생성
	if (ctx == NULL) return false; //<초기화 실패 시 과정 종료
	if (EVP_EncryptInit(ctx, algo, key, iv) == 0) return false;//<사이퍼 컨택스트 초기화 작업. aes 256 ofb 모드를 사용한다. 
	if (EVP_EncryptUpdate(ctx, out, &encLen, in, inLen) == 0) return false;//<암호화를 진행한다.  //*outLen = encLen;
	*outLen = encLen; //길이 저장
	if (EVP_EncryptFinal_ex(ctx, out + encLen, &encLen) == 0) return false;//<암호화가 되지 않은 마지막 데이터를 패딩을 씌어 암호화를 진행하는 함수.
	*outLen += encLen; // 나머지 길이도 저장
	EVP_CIPHER_CTX_free(ctx);//<사이퍼 컨텍스트 메모리 해제 
	return true;
}

bool __OPENSSL__::decrypt(IN BYTE* key, IN BYTE* iv, IN BYTE* in, IN int inLen, OUT BYTE* out, OUT PSIZE_T outLen, IN const EVP_CIPHER * algo)
{
	int addLen; //복호화된 길이 저장 변수
	EVP_CIPHER_CTX *ctx = EVP_CIPHER_CTX_new();//<사이퍼 컨텍스트 생성
	if (ctx == NULL) {
		return false; //<초기화 실패 시 과정 종료
	}
	if (EVP_DecryptInit(ctx, algo, key, iv) == 0) {
		return false;//<사이퍼 컨택스트 초기화 작업. aes 256 ecb 모드를 사용한다. 
	}
	if (EVP_DecryptUpdate(ctx, out, &addLen, in, inLen) == 0) {
		return false;//<복호화를 진행한다.
	}
	*outLen = addLen; //길이 저장
	if (EVP_DecryptFinal_ex(ctx, out + addLen, &addLen) == 0) {
		return false;
	}
	*outLen += addLen; // 나머지 길이도 저장

	EVP_CIPHER_CTX_free(ctx);//<사이퍼 컨텍스트 메모리 해제
	return true;
}

void __OPENSSL__::Base64E(IN const BYTE* in, IN int in_len, OUT char** out, OUT PSIZE_T out_len)
{
	BUF_MEM *ptr;
	BIO* b64f = BIO_new(BIO_f_base64());
	BIO* bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64f, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, in, in_len);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &ptr);

	(*out) = (char *)OPENSSL_malloc((ptr->length + 1) * sizeof(char));
	memcpy(*out, ptr->data, ptr->length);
	(*out)[ptr->length] = '\0';
	*out_len = ptr->length;
	BIO_set_close(bio, BIO_CLOSE);
	BIO_free_all(bio);
}


void __OPENSSL__::Base64E(IN const BYTE* in, IN int in_len, OUT BYTE** out, OUT PSIZE_T out_len)
{
	BUF_MEM *ptr;
	BIO* b64f = BIO_new(BIO_f_base64());
	BIO* bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64f, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, in, in_len);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &ptr);

	(*out) = (BYTE *)OPENSSL_malloc((ptr->length + 1) * sizeof(char));
	memcpy(*out, ptr->data, ptr->length);
	(*out)[ptr->length] = '\0';
	*out_len = ptr->length;
	BIO_set_close(bio, BIO_CLOSE);
	BIO_free_all(bio);
}


void __OPENSSL__::Base64E2(IN const void* in, IN int in_len, OUT char** out, OUT PSIZE_T out_len)
{
	BUF_MEM *ptr;
	BIO* b64f = BIO_new(BIO_f_base64());
	BIO* bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64f, bio);
	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL);
	BIO_write(bio, in, in_len);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &ptr);

	(*out) = (char *)OPENSSL_malloc((ptr->length + 1) * sizeof(char));
	memcpy(*out, ptr->data, ptr->length);
	(*out)[ptr->length] = '\0';
	*out_len = ptr->length;
	BIO_set_close(bio, BIO_CLOSE);
	BIO_free_all(bio);
}


void __OPENSSL__::Base64D(IN char *input, IN size_t inLen, OUT char **buffer, OUT PINT length)
{
	size_t decodeLen = calcDecodeLength(input, inLen);
	(*buffer) = (char*)OPENSSL_malloc(decodeLen + 1);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO *bmem = BIO_new_mem_buf(input, (int)inLen);
	bmem = BIO_push(b64, bmem);
	BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
	BIO_set_close(b64, BIO_CLOSE);
	//BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

	*length = BIO_read(bmem, *buffer, (int)decodeLen + 1);
	(*buffer)[decodeLen] = '\0';
	BIO_free_all(bmem);
}

void __OPENSSL__::Base64D(IN BYTE *input, IN size_t inLen, OUT BYTE **buffer, OUT PINT length)
{
	size_t decodeLen = calcDecodeLength(input, inLen);
	(*buffer) = (BYTE*)OPENSSL_malloc(decodeLen + 1);
	BIO *b64 = BIO_new(BIO_f_base64());
	BIO *bmem = BIO_new_mem_buf(input, (int)inLen);
	bmem = BIO_push(b64, bmem);
	BIO_set_flags(bmem, BIO_FLAGS_BASE64_NO_NL);
	BIO_set_close(b64, BIO_CLOSE);
	//BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);

	*length = BIO_read(bmem, *buffer, (int)decodeLen + 1);
	(*buffer)[decodeLen] = '\0';
	BIO_free_all(bmem);
}

size_t __OPENSSL__::calcDecodeLength(const BYTE* b64input, size_t len) { //Calculates the length of a decoded string
//	size_t len = strlen(b64input);
	size_t padding = 0;
	if (b64input[len - 1] == '=' && b64input[len - 2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len - 1] == '=') //last char is =
		padding = 1;
	return (len * 3) / 4 - padding;
}

size_t __OPENSSL__::calcDecodeLength(const char* b64input, size_t len) { //Calculates the length of a decoded string
	size_t padding = 0;
	if (b64input[len - 1] == '=' && b64input[len - 2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len - 1] == '=') //last char is =
		padding = 1;
	return (len * 3) / 4 - padding;
}

bool __OPENSSL__::hash(char* in, BYTE ** out, const EVP_MD* algo, int md_len) {
	UINT outLen;
	*out = (BYTE*)OPENSSL_malloc(EVP_MD_size(algo));
	EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
	if (mdctx == NULL) return false; // mdctx 초기화
	if (EVP_DigestInit(mdctx, algo) == 0) return false;//<파라메타로 들어온 해시 타입으로 메시지 다이제스트 컨텍스트 초기화
	if (EVP_DigestUpdate(mdctx, in, strlen(in)) == 0) return false;//<해시 진행.
	if (EVP_DigestFinal_ex(mdctx, *out, &outLen) == 0) return false; //실질적으로 해시 진행하는듯
	EVP_MD_CTX_destroy(mdctx);
	return true;
}

const EVP_CIPHER* __OPENSSL__::getCipher(const char* algo,const char* mode) {
	//return EVP_get_cipherbyname(str);
	__CHECK__ check;
	if (check.isEqual("seed", algo)) {
		if (check.isEqual("cfb", mode)) {
			return EVP_seed_cfb();
		}
		else if (check.isEqual("ofb", mode)) {
			return EVP_seed_ofb();
		}
		else{
			return EVP_seed_cbc();
		}
	}
	else if (check.isEqual("aes", algo)) {
		if (check.isEqual("ctr", mode)) {
			return EVP_aes_256_ctr();
		}
		else if (check.isEqual("ofb", mode)) {
			return EVP_aes_256_ofb();
		}
		else if (check.isEqual("cfb", mode)) {
			return EVP_aes_256_cfb();
		}
		else {
			return EVP_aes_256_cbc();
		}
	}
	else return NULL;
}

const EVP_MD* __OPENSSL__::getMd(const char* algo, PINT LENGTH) {
	//return EVP_get_digestbyname("sha256");
	__CHECK__ check;
	if (check.isEqual("256", algo)) {
		*LENGTH = SHA256_DIGEST_LENGTH;
		return EVP_sha256();
	}
	else if (check.isEqual("384", algo)) {
		*LENGTH = SHA384_DIGEST_LENGTH;
		return EVP_sha384();
	}
	else if (check.isEqual("512", algo)) {
		*LENGTH = SHA512_DIGEST_LENGTH;
		return EVP_sha512();
	}
	else return NULL;
}


/* KISA Library */

void __KISA__::DL(const BYTE *i, BYTE *o) // Diffusion Layer
{
	BYTE T;

	T = i[3] ^ i[4] ^ i[9] ^ i[14];
	o[0] = i[6] ^ i[8] ^ i[13] ^ T;
	o[5] = i[1] ^ i[10] ^ i[15] ^ T;
	o[11] = i[2] ^ i[7] ^ i[12] ^ T;
	o[14] = i[0] ^ i[5] ^ i[11] ^ T;
	T = i[2] ^ i[5] ^ i[8] ^ i[15];
	o[1] = i[7] ^ i[9] ^ i[12] ^ T;
	o[4] = i[0] ^ i[11] ^ i[14] ^ T;
	o[10] = i[3] ^ i[6] ^ i[13] ^ T;
	o[15] = i[1] ^ i[4] ^ i[10] ^ T;
	T = i[1] ^ i[6] ^ i[11] ^ i[12];
	o[2] = i[4] ^ i[10] ^ i[15] ^ T;
	o[7] = i[3] ^ i[8] ^ i[13] ^ T;
	o[9] = i[0] ^ i[5] ^ i[14] ^ T;
	o[12] = i[2] ^ i[7] ^ i[9] ^ T;
	T = i[0] ^ i[7] ^ i[10] ^ i[13];
	o[3] = i[5] ^ i[11] ^ i[14] ^ T;
	o[6] = i[2] ^ i[9] ^ i[12] ^ T;
	o[8] = i[1] ^ i[4] ^ i[15] ^ T;
	o[13] = i[3] ^ i[6] ^ i[8] ^ T;
}
// Right-rotate 128 bit source string s by n bits and XOR it to target string t
void __KISA__::RotXOR(const BYTE *s, int n, BYTE *t)
{
	int i, q;

	q = n / 8; n %= 8;
	for (i = 0; i < 16; i++) {
		t[(q + i) % 16] ^= (s[i] >> n);
		if (n != 0) t[(q + i + 1) % 16] ^= (s[i] << (8 - n));
	}
}

// Encryption round key generation rountine
// w0 : master key, e : encryption round keys
int __KISA__::EncKeySetup(const BYTE *w0, BYTE *e, int keyBits) {
	int  i, R = (keyBits + 256) / 32, q;
	BYTE t[16], w1[16], w2[16], w3[16];

	q = (keyBits - 128) / 64;
	for (i = 0; i < 16; i++) t[i] = S[i % 4][KRK[q][i] ^ w0[i]];
	DL(t, w1);
	if (R == 14)
		for (i = 0; i < 8; i++) w1[i] ^= w0[16 + i];
	else if (R == 16)
		for (i = 0; i < 16; i++) w1[i] ^= w0[16 + i];

	q = (q == 2) ? 0 : (q + 1);
	for (i = 0; i < 16; i++) t[i] = S[(2 + i) % 4][KRK[q][i] ^ w1[i]];
	DL(t, w2);
	for (i = 0; i < 16; i++) w2[i] ^= w0[i];

	q = (q == 2) ? 0 : (q + 1);
	for (i = 0; i < 16; i++) t[i] = S[i % 4][KRK[q][i] ^ w2[i]];
	DL(t, w3);
	for (i = 0; i < 16; i++) w3[i] ^= w1[i];

	for (i = 0; i < 16 * (R + 1); i++) e[i] = 0;

	RotXOR(w0, 0, e); RotXOR(w1, 19, e);
	RotXOR(w1, 0, e + 16); RotXOR(w2, 19, e + 16);
	RotXOR(w2, 0, e + 32); RotXOR(w3, 19, e + 32);
	RotXOR(w3, 0, e + 48); RotXOR(w0, 19, e + 48);
	RotXOR(w0, 0, e + 64); RotXOR(w1, 31, e + 64);
	RotXOR(w1, 0, e + 80); RotXOR(w2, 31, e + 80);
	RotXOR(w2, 0, e + 96); RotXOR(w3, 31, e + 96);
	RotXOR(w3, 0, e + 112); RotXOR(w0, 31, e + 112);
	RotXOR(w0, 0, e + 128); RotXOR(w1, 67, e + 128);
	RotXOR(w1, 0, e + 144); RotXOR(w2, 67, e + 144);
	RotXOR(w2, 0, e + 160); RotXOR(w3, 67, e + 160);
	RotXOR(w3, 0, e + 176); RotXOR(w0, 67, e + 176);
	RotXOR(w0, 0, e + 192); RotXOR(w1, 97, e + 192);
	if (R > 12) {
		RotXOR(w1, 0, e + 208); RotXOR(w2, 97, e + 208);
		RotXOR(w2, 0, e + 224); RotXOR(w3, 97, e + 224);
	}
	if (R > 14) {
		RotXOR(w3, 0, e + 240); RotXOR(w0, 97, e + 240);
		RotXOR(w0, 0, e + 256); RotXOR(w1, 109, e + 256);
	}
	return R;
}

// Decryption round key generation rountine
// w0 : maskter key, d : decryption round keys
int __KISA__::DecKeySetup(const BYTE *w0, BYTE *d, int keyBits) {
	int  i, j, R;
	BYTE t[16];

	R = EncKeySetup(w0, d, keyBits);
	for (j = 0; j < 16; j++) {
		t[j] = d[j];
		d[j] = d[16 * R + j];
		d[16 * R + j] = t[j];
	}
	for (i = 1; i <= R / 2; i++) {
		DL(d + i * 16, t);
		DL(d + (R - i) * 16, d + i * 16);
		for (j = 0; j < 16; j++) d[(R - i) * 16 + j] = t[j];
	}
	return R;
}

// Encryption and decryption rountine
// p: plain text, e: round keys, c: ciphertext
void __KISA__::Crypt(const BYTE *p, int R, const BYTE *e, BYTE *c)
{
	int i, j;
	BYTE t[16];

	for (j = 0; j < 16; j++) c[j] = p[j];
	for (i = 0; i < R / 2; i++)
	{
		for (j = 0; j < 16; j++) t[j] = S[j % 4][e[j] ^ c[j]];
		DL(t, c); e += 16;
		for (j = 0; j < 16; j++) t[j] = S[(2 + j) % 4][e[j] ^ c[j]];
		DL(t, c); e += 16;
	}
	DL(c, t);
	for (j = 0; j < 16; j++) c[j] = e[j] ^ t[j];
}

void __KISA__::printBlockOfLength(BYTE *b, int len) {
#ifdef _DEBUG
	for (int i = 0; i<len; i++, b++) {
		printf("0x%02x ", *b);
		if (i % 4 == 3 && i<len - 1) printf("\n");
	}
	printf("\n");	
#endif
}

void __KISA__::printBlock(BYTE *b) {
	printBlockOfLength(b, 16);
}

bool __KISA__::encrypt(BYTE* input,BYTE* key, BYTE* iv, size_t inLen, PSIZE_T opLen, BYTE* output)
{
	BYTE rk[16 * 17];
	int paddingSize;
	size_t count = __splitc(inLen,&paddingSize);
	BYTE** buff = __split(input, count);
	const int start = 16 - paddingSize;
	addPadding(buff[count-1], (char)paddingSize, start, 16);
	for(int i=0;i<count;i++){
		Crypt(buff[i], EncKeySetup(key, rk ,256), rk, output+(i*16));
	}
	//printBlock(buff[count - 1]);
	__split_clear(buff, count);
	*opLen = count*16;
	return true;
}

bool __KISA__::decrypt(BYTE* input,BYTE* key, BYTE* iv, size_t inLen, PSIZE_T opLen, BYTE** output)
{
	BYTE rk[16 * 17];
	
	size_t count = __splitc(inLen, NULL);
	
	BYTE* out = (BYTE*)OPENSSL_malloc(count * 16 +1);
	//(*output) = (BYTE*)OPENSSL_malloc(count * 16);
	//printf("\ncount: %zd\n", count);
	BYTE** buff = __split(input, count);
	
	for (int i = 0; i<count; i++) {
		Crypt(buff[i], DecKeySetup(key, rk, 256), rk, out + (i * 16));
	}
	__split_clear(buff, count);
	BYTE** buff2 = __split(out, count);
	OPENSSL_free(out);

	char start = buff2[count - 1][16-1]; // find padding buffer
	delPadding(buff2[count -1], 16-(int)start, 16);
	*opLen = count * 16 - (int)start;
	*output = (BYTE*)OPENSSL_malloc(count*16+1);
	__merge(buff2, count, *output);
	
	__split_clear(buff2, 16);
	return true;
}

void __KISA__::addPadding(BYTE *pBuffer, BYTE pChar, int start, int end) {
	for (int i = start; i<end; i++) {
		pBuffer[i] = pChar;
	}
}

void __KISA__::delPadding(BYTE *pBuffer, int start, int end) {
	for (int i = start; i<end; i++) {
		pBuffer[i] = 0;
	}
}

size_t __KISA__::__splitc(size_t inLen, PINT padSize) {
	const size_t count = (inLen % 16) == 0 ? inLen / 16 : inLen / 16 + 1;
	if (padSize != NULL) {
		*padSize = (inLen % 16) == 0 ? 0 : 16 - ((int)inLen % 16);
	}
	return count;
}

BYTE** __KISA__::__split(BYTE* input,size_t count) {
	BYTE** temp;
	temp = (BYTE**)OPENSSL_malloc(sizeof(BYTE*)*count+1);
	for (size_t i = 0; i < count; i++) {
		temp[i] = (BYTE*)OPENSSL_malloc(16+1);
		memcpy(temp[i],input+(i*16),16);
	}
	return temp;
}

void __KISA__::__split_clear(BYTE** input,size_t count) {
	for (size_t i = 0; i < count; i++) {
		OPENSSL_free(input[i]);
	}
	OPENSSL_free(input);	
}

void __KISA__::__merge(IN BYTE** in, IN size_t count, OUT BYTE* out) { //여기다가 입력값 받을꺼 변수 추가
	for (size_t i = 0; i < count; i++)
		memcpy(out + (i * 16), in[i], 16);
}

/*

// 사용안함
const char* __OPENSSL__::hash_disable(char* in, const EVP_MD* algo, int md_len) {
UINT outLen;
BYTE* hash = new BYTE[md_len]();
EVP_MD_CTX *mdctx = EVP_MD_CTX_create();
if (mdctx == NULL) return "er1";
if (EVP_DigestInit(mdctx, algo) == NULL) return "er2";//<파라메타로 들어온 해시 타입으로 메시지 다이제스트 컨텍스트 초기화
if (EVP_DigestUpdate(mdctx, in, strlen(in)) == NULL) return "er3";//<해시 진행.

if (EVP_DigestFinal_ex(mdctx, hash, &outLen) == NULL) return "er4";
char* output = new char[(md_len*2)+1]();
for (int i = 0; i < md_len; i++)
sprintf_s((char*)&output[i * 2], md_len, "%02x", (UINT)hash[i]);
EVP_MD_CTX_destroy(mdctx);
delete[] hash;
return output;
//return true;
}



bool __KISA__::encrypt(BYTE* input,BYTE* key, BYTE* iv, size_t inLen, size_t opLen, BYTE* output)
{
//여기는 따로 짜야될듯 암호화 돌리는거 사이즈 동적어쩌고 하는거로 아마

BYTE rk[16 * 17], mk[32];


memcpy(mk, iv, 16);
//memcpy(mk, iv, 32);
/*
for (int i = 0; i<16; i++)
mk[i] = i * 0x11;
for (int i = 16; i<24; i++)
mk[i] = (i - 16) * 0x11;

//mk가 변경되어야 하는데 어떻게 자동생성하지...
int paddingSize;
size_t count = __splitc(inLen, &paddingSize);
BYTE** buff = __split(input, count);
//BYTE *buffer = (BYTE*)OPENSSL_malloc(16);
//memcpy(buffer, input, inLen);
//BYTE* c = (BYTE*)OPENSSL_malloc((count-1)*16);
int start = 16 - paddingSize;
addPadding(buff[count - 1], (char)paddingSize, start, 16);
for (int i = 0; i<count; i++) {
	Crypt(buff[i], EncKeySetup(iv, key, 256), key, output + (i * 16));
}

__split_clear(buff, count);

//printf("[result]\n");  ki.printBlock(c); printf("\n");
//ki.Crypt(c, ki.DecKeySetup(mk, rk, 256), rk, out);	
//delPadding(out, start, 16);
opLen = count * 16;
//		printf("[result]\n"); ki.printBlock(out); printf("\n");
//OPENSSL_free(buffer);
return true;
}

bool __KISA__::decrypt(BYTE* input, BYTE* key, BYTE* iv, size_t inLen, size_t opLen, BYTE* output)
{

	BYTE rk[16 * 17], mk[32];
	
	for (int i = 0; i<16; i++)
	mk[i] = i * 0x11;
	for (int i = 16; i<24; i++)
	mk[i] = (i - 16) * 0x11;
	
	//mk가 변경되어야 하는데 어떻게 자동생성하지...
	memcpy(mk, iv, 16);
	//memcpy(mk, key, 32);
	int paddingSize;
	size_t count = __splitc(inLen, &paddingSize);
	BYTE** buff = __split(input, count);
	int start = 16 - paddingSize;
	delPadding(buff[count - 1], start, 16);
	for (int i = 0; i<count; i++) {
		Crypt(buff[i], DecKeySetup(iv, key, 256), key, output + (i * 16));
	}

	__split_clear(buff, count);
	opLen = count * 16;
	return true;
}


*/


void encrypt(string fileName, string content)
{
	__EDH__ edh;
	BYTE iv[] = "d539vmci0kr190i5";
	BYTE key[256 / 8 + 1] = "17CA77291E3038265226CE3ED2594C5D"; //AES, ARIA
																//BYTE key[128 / 8] = "17CA77291E30382";// SEED
	size_t size_d = 0;
	BYTE *inC = NULL;
	PSIZE_T size = (PSIZE_T)&size_d;
	char out_c = ' ';
	char* out = &out_c;
	size_t outLen_d = 0;
	PSIZE_T outLen = (PSIZE_T)&outLen_d;
	char* hash;

	edh.hash((char*)content.c_str(), "256", &hash);
	if (edh.encrypt("aes", "cbc", key, iv, (BYTE*)content.c_str(), content.size(), &out, outLen)) {
		//string encFileName = fileName + ".enc";
		if (edh.writeFile(const_cast<char*>(fileName.c_str()), out, *outLen)) {
		}
		else printf("Fail\n");
	}
	else {
		printf("Fail\n");
	}
}

void decrypt(string fileName)
{
	__EDH__ edh;
	BYTE iv[] = "d539vmci0kr190i5";
	BYTE key[256 / 8 + 1] = "17CA77291E3038265226CE3ED2594C5D"; //AES, ARIA
																//BYTE key[128 / 8] = "17CA77291E30382";// SEED

	BYTE* inCd = NULL;

	size_t sized_d = 0;
	PSIZE_T sized = (PSIZE_T)&sized_d;

	if (edh.readFile(const_cast<char*>(fileName.c_str()), &inCd, sized)) {
		char decplain_d = ' ';
		char *decplain = &decplain_d;
		size_t decoutLen_d = 0;
		PSIZE_T decoutLen = (PSIZE_T)&decoutLen_d;
		if (edh.decrypt("aes", "cbc", key, iv, inCd, *sized, &decplain, decoutLen)) {
			//free(decplain);
			//...
			if (edh.writeFile(const_cast<char*>(fileName.c_str()), decplain, *decoutLen)) {
			}
		}
		else {
			printf("Fail\n");
		}
	}
}


