#include "codec.h"
#include "query.pb.h"

#include <cstdio>

void print(const std::string& buf) {
    printf("encoded to %zd bytes\n", buf.size());
    for (size_t i=0; i<buf.size(); i++) {
        printf("%2zd:  0x%02x  %c\n",
               i,
               (unsigned char)buf[i],
               isgraph(buf[i]) ? buf[i] : ' ');
    }
}

void testQuery() {
    wave::Query query;
    query.set_id(1);
    query.set_questioner("wavestyle");
    query.add_question("Running?");

    std::string transport = encode(query);
    print(transport);

    int32_t len = 0;
    std::copy(transport.begin(), transport.begin() + sizeof len, reinterpret_cast<char*>(&len));
    assert(len == (int32_t)(transport.size() - sizeof(int32_t)));

    std::string buf = transport.substr(sizeof(int32_t));
    assert(len == (int32_t)(buf.size()));

    wave::Query* newQuery = dynamic_cast<wave::Query*>(decode(buf));
    assert(newQuery);
    newQuery->PrintDebugString();
    assert(newQuery->DebugString() == query.DebugString());

    delete newQuery;

    buf[buf.size() - 6]++;
    wave::Query* badQuery = dynamic_cast<wave::Query*>(decode(buf));
    assert(badQuery == nullptr);
}


void testEmpty() {
    wave::Empty empty;

    std::string transport = encode(empty);
    print(transport);

    std::string buf = transport.substr(sizeof(int32_t));

    wave::Empty* newEmpty = dynamic_cast<wave::Empty*>(decode(buf));
    assert(newEmpty != nullptr);
    newEmpty->PrintDebugString();
    assert(newEmpty->DebugString() == empty.DebugString());
    delete newEmpty;
}

void testAnswer() {
    wave::Answer answer;
    answer.set_id(1);
    answer.set_questioner("wavestyle");
    answer.set_answerer("hello_world");
    answer.add_solution("lua");
    answer.add_solution("c++");
    answer.add_solution("python");

    std::string transport = encode(answer);
    print(transport);

    int32_t len = 0;
    std::copy(transport.begin(), transport.begin() + sizeof(int32_t), reinterpret_cast<char*>(&len));
    assert(len == (int32_t)(transport.size() - sizeof(len)));


    std::string buf = transport.substr(sizeof(int32_t));
    assert(len == (int32_t)buf.size());

    wave::Answer* newAnswer = dynamic_cast<wave::Answer*>(decode(buf));
    assert(newAnswer != nullptr);
    newAnswer->PrintDebugString();
    assert(newAnswer->DebugString() == answer.DebugString());

    delete newAnswer;

    buf[buf.size() - 6]++;
    wave::Answer* badAnswer = dynamic_cast<wave::Answer*>(decode(buf));
    assert(badAnswer == nullptr);
}

int main() {
    GOOGLE_PROTOBUF_VERIFY_VERSION;

    testQuery();
    puts("");
    testAnswer();
    puts("");
    testEmpty();
    puts("");

    puts("All pass!!!");

    google::protobuf::ShutdownProtobufLibrary();
}
