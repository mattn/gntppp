#ifndef gntp_h
#define gntp_h

#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 0
#include <sstream>
#include <iostream>
#include <string>
#include <cryptopp/osrng.h>
#include <cryptopp/files.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#include <cryptopp/md5.h>
#include <cryptopp/des.h>
#include <cryptopp/aes.h>
#include <cryptopp/filters.h>
#include <cryptopp/modes.h>

#include <asio.hpp>

template<class CIPHER_TYPE = CryptoPP::DES, class HASH_TYPE = CryptoPP::MD5>
class gntp {
private:
  static inline std::string to_hex(CryptoPP::SecByteBlock& in) {
    std::string out;
    CryptoPP::HexEncoder hex( NULL, true, 2, "" );
    hex.Attach(new CryptoPP::StringSink(out));
    hex.PutMessageEnd(in.begin(), in.size());
    return out;
  }

  void send(const char* method, std::stringstream& stm) {
    asio::ip::tcp::iostream sock(hostname_, port_);
	if (!sock) return;

    // initialize salt and iv
    CryptoPP::SecByteBlock salt(8), iv(8);
    rng.GenerateBlock(salt.begin(), salt.size());
    rng.GenerateBlock(iv.begin(), iv.size());

    if (!password_.empty()) {
      // get digest of password+salt hex encoded
      CryptoPP::SecByteBlock passtext(HASH_TYPE::DIGESTSIZE);
      HASH_TYPE hash;
      hash.Update((byte*)password_.c_str(), password_.size());
      hash.Update(salt.begin(), salt.size());
      hash.Final(passtext);
      CryptoPP::SecByteBlock digest(HASH_TYPE::DIGESTSIZE);
      hash.CalculateDigest(digest.begin(), passtext.begin(), passtext.size());

      class CryptoPP::CBC_Mode<CIPHER_TYPE>::Encryption
        encryptor(passtext.begin(), iv.size(), iv.begin());

      std::string cipher_text;
      CryptoPP::StringSource(stm.str(), true,
        new CryptoPP::StreamTransformationFilter(encryptor,
        new CryptoPP::StringSink(cipher_text)
        ) // StreamTransformationFilter
      ); // StringSource

      sock << "GNTP/1.0 "
        << method
        << " " << CIPHER_TYPE::StaticAlgorithmName() << ":" << to_hex(iv)
        << " " << HASH_TYPE::StaticAlgorithmName() << ":" << to_hex(digest) << "." << to_hex(salt)
        << "\r\n"
        << cipher_text << "\r\n\r\n";
    } else {
      sock << "GNTP/1.0 "
        << method
        << " NONE\r\n"
        << stm.str() << "\r\n";
    }

    while (1) {
      std::string line;
      if (!std::getline(sock, line)) {
        break;
      }
      //std::cout << "[" << line << "]" << std::endl;
      if (line.find("GNTP/1.0 -ERROR") == 0)
        throw "failed to register notification";
      if (line == "\r") break;
    }
  }

  std::string application_;
  std::string hostname_;
  std::string port_;
  std::string password_;
  CryptoPP::AutoSeededRandomPool rng;
public:
  gntp(std::string application = "gntp-send", std::string password = "",
      std::string hostname = "localhost", std::string port = "23053") :
    application_(application),
    password_(password),
    hostname_(hostname),
    port_(port) { }

  void regist(const char* name) {
    std::stringstream stm;
    stm << "Application-Name: " << application_ << "\r\n";
    stm << "Notifications-Count: 1\r\n";
    stm << "\r\n";
    stm << "Notification-Name: " << name << "\r\n";
    stm << "Notification-Display-Name: " << name << "\r\n";
    stm << "Notification-Enabled: True\r\n";
    stm << "\r\n";
    send("REGISTER", stm);
  }

  void notify(const char* name, const char* title, const char* text, const char* icon = NULL) {
    std::stringstream stm;
    stm << "Application-Name: " << application_ << "\r\n";
    stm << "Notification-Name: " << name << "\r\n";
    if (icon) stm << "Notification-Icon: " << icon << "\r\n";
    stm << "Notification-Title: " << title << "\r\n";
    stm << "Notification-Text: " << text << "\r\n";
    stm << "\r\n";
    send("NOTIFY", stm);
  }
};

#endif
