#include "gntp.h"

using namespace CryptoPP;

int main(void) {
  try {
    gntp client("my growl application", "my-password-is-secret");
    //client.regist("my-event");
    client.regist<DES, MD5>("my-event");
    client.notify("my-event", "タイトル", "本文", "http://mattn.kaoriya.net/images/logo.png");

	// currently, AES does not work... Hmmm
    //client.notify<AES, SHA1>("my-event", "タイトル", "本文", "http://mattn.kaoriya.net/images/logo.png");
  } catch(const std::exception& e) {
    std::cerr << e.what() << std::endl;
  }
  return 0;
}
