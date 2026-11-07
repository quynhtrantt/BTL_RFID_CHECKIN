#include "app_system.h"
#include "driver_mcp79410.h"
#include <string.h>
/* EEPROM: password banks 0..15; 8 card records 16..71; 2 logs 72..127.
   Name IDs refer to the fixed seed-name dictionary; new cards have no name.
   Logs contain independent name snapshots, surviving user deletion. */
static const char names[][APP_NAME_SIZE]={"", "Nguyen An", "Tran Binh", "Le Chi"};
static uint8_t image[128], password_bank;
static bool ready;
static const char *storage_error="Chua khoi tao";
static uint8_t crc8(const uint8_t *p,unsigned n) {
    uint8_t c=0xA7;
    while(n--) { c^=*p++; for(unsigned i=0;i<8;i++) c=(c&0x80)?(uint8_t)((c<<1)^7):(uint8_t)(c<<1); }
    return c;
}
static uint16_t crc16(const uint8_t *p,unsigned n) {
    uint16_t c=0xFFFF;
    while(n--) { c^=(uint16_t)*p++<<8; for(unsigned i=0;i<8;i++) c=(c&0x8000)?(uint16_t)((c<<1)^0x1021):(uint16_t)(c<<1); }
    return c;
}
static uint32_t get32(const uint8_t *p) {
    return (uint32_t)p[0]|(uint32_t)p[1]<<8|(uint32_t)p[2]<<16|(uint32_t)p[3]<<24;
}
static void set32(uint8_t *p,uint32_t v) { for(unsigned i=0;i<4;i++) p[i]=(uint8_t)(v>>(8*i)); }
static bool digits(const char *p) {
    if(!p || strlen(p)!=APP_PASS_LENGTH) return false;
    for(unsigned i=0;i<APP_PASS_LENGTH;i++) if(p[i]<'0'||p[i]>'9') return false;
    return true;
}
static bool pass_ok(const uint8_t *p) {
    for(unsigned i=1;i<=6;i++) if(p[i]<'0'||p[i]>'9') return false;
    return p[7]==crc8(p,7);
}
static bool card_ok(unsigned i) {
    const uint8_t *p=image+16+i*7;
    return i<APP_USER_COUNT && p[5]<sizeof(names)/sizeof(names[0]) && p[6]==crc8(p,6);
}
static bool log_ok(unsigned i) {
    const uint8_t *p=image+72+i*28;
    return p[25]==0xA5 && p[11]==0 &&
        ((uint16_t)p[26]|(uint16_t)p[27]<<8)==crc16(p,26);
}
static bool write_record(unsigned addr,const uint8_t *p,unsigned len) {
    if(!Driver_MCP_WriteEEPROM((uint8_t)addr,p,(uint16_t)len)) {
        /* Require restart/reload after an uncertain write. Never report success. */
        ready=false; storage_error=Driver_MCP_ErrorText(); return false;
    }
    memcpy(image+addr,p,len); return true;
}
void App_System_InitDB(void) {
    ready=false;
    storage_error="Khong doc 0x57";
    if(!Driver_MCP_ReadEEPROM(0,image,128)) { storage_error=Driver_MCP_ErrorText(); return; }
    if(!Driver_MCP_PrepareEEPROM()) { storage_error=Driver_MCP_ErrorText(); return; }
    bool a=pass_ok(image),b=pass_ok(image+8);
    if(a||b) {
        password_bank=(!a || (b && (int8_t)(image[8]-image[0])>0))?1:0;
        ready=true; storage_error="OK"; return;
    }
    /* Provision only an erased/zeroed device, not a corrupted existing password. */
    bool erased=true,zero=true;
    for(unsigned i=0;i<128;i++) { erased &= image[i]==0xFF; zero &= image[i]==0; }
    storage_error="Du lieu/pass loi";
    if(!erased && !zero) return;
    storage_error="Ghi/kiem tra loi";
    memset(image,0xFF,sizeof(image));
    if(!Driver_MCP_WriteEEPROM(0,image,128)) { storage_error=Driver_MCP_ErrorText(); return; }
    static const uint8_t ids[]={1,5,16};
    for(unsigned i=0;i<3;i++) {
        uint8_t p[]={1,2,3,4,ids[i],(uint8_t)(i+1),0}; p[6]=crc8(p,6);
        if(!Driver_MCP_WriteEEPROM(16+i*7,p,7)) { storage_error=Driver_MCP_ErrorText(); return; }
        memcpy(image+16+i*7,p,7);
    }
    uint8_t p[]={0,'1','2','3','4','5','6',0}; p[7]=crc8(p,7);
    if(!Driver_MCP_WriteEEPROM(0,p,8)) { storage_error=Driver_MCP_ErrorText(); return; }
    memcpy(image,p,8); password_bank=0; ready=true; storage_error="OK";
}
bool App_System_Ready(void) { return ready; }
const char *App_System_ErrorText(void) { return storage_error; }
int App_System_FindCard(RFID_UID_t *card) {
    if(!ready || !card || card->size!=5) return -1;
    for(unsigned i=0;i<APP_USER_COUNT;i++)
        if(card_ok(i) && !memcmp(image+16+i*7,card->uid,5)) return (int)i;
    return -1;
}
bool App_System_VerifyCard(RFID_UID_t *card) { return App_System_FindCard(card)>=0; }
bool App_System_AddCard(RFID_UID_t *card) {
    if(!ready||!card||card->size!=5) return false;
    if(App_System_FindCard(card)>=0) return false;
    for(unsigned i=0;i<APP_USER_COUNT;i++) if(!card_ok(i)) {
        uint8_t p[7]; memcpy(p,card->uid,5); p[5]=0; p[6]=crc8(p,6);
        return write_record(16+i*7,p,7);
    }
    return false;
}
bool App_System_DeleteUser(int i) {
    if(!ready||i<0||i>=APP_USER_COUNT||!card_ok((unsigned)i)) return false;
    uint8_t p[7]; memset(p,0xFF,7);
    return write_record(16+i*7,p,7);
}
void App_System_DeleteCard(int i) { (void)App_System_DeleteUser(i); }
bool App_System_GetUser(int i,AppUser_t *u) {
    if(!ready||!u||i<0||i>=APP_USER_COUNT||!card_ok((unsigned)i)) return false;
    memcpy(u->card.uid,image+16+i*7,5); u->card.size=5;
    memcpy(u->name,names[image[16+i*7+5]],APP_NAME_SIZE); return true;
}
int App_System_NextUser(int previous) {
    if(!ready) return -1;
    for(int n=1;n<=APP_USER_COUNT;n++) {
        int i=(previous+n)%APP_USER_COUNT;
        if(i>=0 && card_ok((unsigned)i)) return i;
    }
    return -1;
}
bool App_System_CheckPassword(const char *p) {
    return ready && digits(p) && !memcmp(p,image+password_bank*8+1,6);
}
bool App_System_ChangePassword(const char *p) {
    if(!ready||!digits(p)) return false;
    unsigned target=1-password_bank;
    uint8_t rec[8]; rec[0]=image[password_bank*8]+1; memcpy(rec+1,p,6); rec[7]=crc8(rec,7);
    if(!write_record(target*8,rec,8)) return false;
    password_bank=(uint8_t)target; return true;
}
bool App_System_LogAttendance(const AppUser_t *u,uint32_t time) {
    if(!ready||!u||u->card.size!=5) return false;
    RFID_UID_t card=u->card;
    if(App_System_FindCard(&card)<0) return false;
    bool a=log_ok(0),b=log_ok(1);
    unsigned target; uint32_t sequence;
    if(!a&&!b) { target=0; sequence=1; }
    else {
        unsigned latest= !a?1:!b?0:((int32_t)(get32(image+121)-get32(image+93))>0?1:0);
        sequence=get32(image+72+latest*28+21)+1;
        target=!a?0:!b?1:1-latest;
    }
    uint8_t rec[28]={0};
    memcpy(rec,u->name,APP_NAME_SIZE-1); memcpy(rec+12,u->card.uid,5);
    set32(rec+17,time); set32(rec+21,sequence); rec[25]=0xA5;
    uint16_t crc=crc16(rec,26); rec[26]=(uint8_t)crc; rec[27]=(uint8_t)(crc>>8);
    /* Invalidate destination first. A torn log is rejected by marker/CRC. */
    uint8_t invalid=0;
    if(!write_record(72+target*28+25,&invalid,1)) return false;
    rec[25]=0;
    if(!write_record(72+target*28,rec,28)) return false;
    uint8_t marker=0xA5;
    return write_record(72+target*28+25,&marker,1);
}
bool App_System_ReadLog(unsigned n,AppLog_t *log) {
    if(!ready||!log||n>=APP_LOG_COUNT) return false;
    bool a=log_ok(0),b=log_ok(1);
    if(!a&&!b) return false;
    unsigned i=!a?1:!b?0:((int32_t)(get32(image+121)-get32(image+93))>0?1:0);
    if(n) { if(!(a&&b)) return false; i=1-i; }
    const uint8_t *p=image+72+i*28;
    memcpy(log->name,p,12); memcpy(log->uid,p+12,5);
    log->time=get32(p+17); log->sequence=get32(p+21); return true;
}
