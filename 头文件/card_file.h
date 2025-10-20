#ifndef CARD_FILE_H
#define CARD_FILE_H
int savecard(const Card* pcard, const char* ppath);
Card  prasecard(char* pbuf);
int getcardcount(const char* ppath);
int readcard(Card* pcard, const char* ppath);
int updata(pcardnode pcard, const char* ppath);



#endif