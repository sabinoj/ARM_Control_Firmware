void wireInit();
void wireSend(char);
void radioInit();
void radioSend(char);
void wireSendString(char*);
void radioSendString(char*);
int wireGetCmpString(volatile unsigned int *, char[]);
