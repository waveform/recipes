#ifndef PROTOBUF_CODEC_H
#define PROTOBUF_CODEC_H

#include <google/protobuf/descriptor.h>
#include <google/protobuf/message.h>
#include <zlib.h>
#include <string>
#include <stdint.h>

/*
   struct ProtobufTransportFormat {
        int32_t len;
        int32_t nameLen;
        char    typeName[nameLen];
        char    protobufData[len-nameLen-8];
        int32_t checkSum;
   }
*/
const int kHeaderLen = sizeof(int32_t);

inline std::string encode(const google::protobuf::Message& message) {
    std::string result;

    result.resize(kHeaderLen);
    const std::string& typeName = message.GetTypeName();
    int32_t nameLen = static_cast<int32_t>(typeName.size()+1);
    result.append(reinterpret_cast<char*>(&nameLen), sizeof(int32_t));
    result.append(typeName.c_str(), nameLen);
    bool succeed = message.AppendToString(&result);
    if (!succeed) { result.clear(); return result; }

    const char* begin = result.c_str() + kHeaderLen;
    int32_t checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), result.size() - kHeaderLen);
    result.append(reinterpret_cast<char*>(&checkSum), sizeof(int32_t));
    int32_t len = result.size() - kHeaderLen;
    std::copy(reinterpret_cast<char*>(&len),
              reinterpret_cast<char*>(&len) + sizeof(len),
              result.begin());

    return result;
}

inline google::protobuf::Message* createMessage(const std::string& type_name) {
    google::protobuf::Message* message = nullptr;

    const google::protobuf::Descriptor* descriptor =
        google::protobuf::DescriptorPool::generated_pool()->FindMessageTypeByName(type_name);
    if (!descriptor) return message;

    const google::protobuf::Message* prototype = google::protobuf::MessageFactory::generated_factory()->GetPrototype(descriptor);
    if (!prototype) return message;

    message = prototype->New();
    return message;
}

inline int32_t asInt32(const char* buf) {
    int32_t ans = 0;
    ::memcpy(&ans, buf, sizeof(int32_t));
    return ans;
}

inline google::protobuf::Message* decode(const std::string& buf) {
    google::protobuf::Message* result = nullptr;

    int32_t len = static_cast<int32_t>(buf.size());
    if (len < 10) return result;

    int32_t expectedCheckSum = asInt32(buf.c_str() + buf.size() - kHeaderLen);
    const char* begin = buf.c_str();
    int32_t checkSum = adler32(1, reinterpret_cast<const Bytef*>(begin), len-kHeaderLen);
    if (checkSum != expectedCheckSum) return result;

    int32_t nameLen = asInt32(buf.c_str());
    if (nameLen < 2 || nameLen > len - 2*kHeaderLen) return result;

    std::string typeName(buf.begin() + kHeaderLen, buf.begin() + kHeaderLen + nameLen - 1);
    google::protobuf::Message* message = createMessage(typeName);
    if (!message) return result;

    const char* data = buf.c_str() + kHeaderLen + nameLen;
    int32_t dataLen = len - nameLen - 2*kHeaderLen;
    if (message->ParseFromArray(data, dataLen)) result = message;
    else delete message;

    return result;
}

#endif//PROTOBUF_CODEC_H
