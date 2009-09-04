#include "gntp.h"

using namespace CryptoPP;

int main(void) {
  //gntp<AES, SHA1> client;
  //gntp<DES, MD5> client;

  gntp<> client("my growl application", "my-password-is-secret");
  client.regist("my-event");
  client.notify("my-event", "タイトル", "本文", "http://mattn.kaoriya.net/images/logo.png");
  return 0;
}
