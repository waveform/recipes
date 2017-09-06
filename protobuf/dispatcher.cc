#include <iostream>
#include <functional>
#include <cassert>
#include <memory>

#include "query.pb.h"

using namespace std;

class Callback {
public:
    //Callback(const Callback&) = delete;
    Callback& operator=(const Callback&) = delete;
    virtual ~Callback() {};
    virtual void onMessage(google::protobuf::Message* message) const = 0;
};

template <typename T>
class CallbackT : public Callback {
public:
    typedef function<void (T* message)> MessageCallback;
    CallbackT(const MessageCallback& callback) : callback_(callback) {  }
    virtual void onMessage(google::protobuf::Message* message) const {
        T* t = dynamic_cast<T*>(message);
        assert(t != nullptr);
        callback_(t);
    }
private:
    MessageCallback callback_;
};

void discardProtobufMessage(google::protobuf::Message* message) {
    cout << "Discarding " << message->GetTypeName() << endl;
}

class ProtobufDispatcher {
public:
    ProtobufDispatcher() : defaultCallback_(discardProtobufMessage) {  }
    void onMessage(google::protobuf::Message* message) const {
        auto it = callbacks_.find(message->GetDescriptor());
        if (it != callbacks_.end()) {
            it->second->onMessage(message);
        } else {
            defaultCallback_(message);
        }
    }
    template<typename T>
    void registerMessageCallback(const typename CallbackT<T>::MessageCallback& callback) {
        callbacks_[T::descriptor()] = make_shared<CallbackT<T>>(callback);
    }

    typedef std::unordered_map<const google::protobuf::Descriptor*, shared_ptr<Callback>> CallbackMap;
    CallbackMap callbacks_;
    function<void (google::protobuf::Message* message)> defaultCallback_;
};

void onQuery(wave::Query* query) {
    cout << "onQuery: " << query->GetTypeName() << endl;
}

void onAnswer(wave::Answer* answer) {
    cout << "onAnswer: " << answer->GetTypeName() << endl;
}

int main() {
    ProtobufDispatcher dispatcher;
    dispatcher.registerMessageCallback<wave::Query>(onQuery);
    dispatcher.registerMessageCallback<wave::Answer>(onAnswer);

    wave::Query q;
    wave::Answer a;
    wave::Empty e;
    dispatcher.onMessage(&q);
    dispatcher.onMessage(&a);
    dispatcher.onMessage(&e);

    google::protobuf::ShutdownProtobufLibrary();
}

