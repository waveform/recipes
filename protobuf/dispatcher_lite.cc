#include <iostream>
#include <functional>
#include <map>

#include "query.pb.h"

using namespace std;

class ProtobufDispatcherLite {
public:
    typedef function<void (google::protobuf::Message* msg)> Callback;
    explicit ProtobufDispatcherLite(const ProtobufDispatcherLite::Callback& defaultCb)
        :defaultCallback_(defaultCb)
    {
    }
    void onMessage(google::protobuf::Message* msg) const {
        CallbackMap::const_iterator it = callbacks_.find(msg->GetDescriptor());
        if (it != callbacks_.end()) {
            it->second(msg);
        } else {
            defaultCallback_(msg);
        }
    }
    void registerMessageCallback(const google::protobuf::Descriptor* desc, const Callback& callback) {
        callbacks_[desc] = callback;
    }
private:
    typedef std::unordered_map<const google::protobuf::Descriptor*, Callback> CallbackMap;
    CallbackMap callbacks_;
    Callback defaultCallback_;
};

void onQuery(google::protobuf::Message* msg) {
    cout << "onQuery: " << msg->GetTypeName() << endl;
    wave::Query* query = dynamic_cast<wave::Query*>(msg);
    assert(query != nullptr);
}

void onAnswer(google::protobuf::Message* msg) {
    cout << "onAnswer: " << msg->GetTypeName() << endl;
    wave::Answer* answer = dynamic_cast<wave::Answer*>(msg);
    assert(answer != nullptr);
}

void onUnknownMessageType(google::protobuf::Message* msg) {
    cout << "Discarding " << msg->GetTypeName() << endl;
}

int main(){
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    ProtobufDispatcherLite dispatcher(onUnknownMessageType);
    dispatcher.registerMessageCallback(wave::Query::descriptor(), onQuery);
    dispatcher.registerMessageCallback(wave::Answer::descriptor(), onAnswer);

    wave::Query q;
    wave::Answer a;
    wave::Empty e;
    dispatcher.onMessage(&q);
    dispatcher.onMessage(&a);
    dispatcher.onMessage(&e);

    google::protobuf::ShutdownProtobufLibrary();
}
