#include <iostream>
#include <future>
#include "signalrclient/hub_connection.h"
#include "signalrclient/hub_connection_builder.h"
#include "signalrclient/signalr_value.h"

class logger : public signalr::log_writer
{
    // Inherited via log_writer
    virtual void write(const std::string& entry) override
    {
        std::cout << entry;
    }
};

int main()
{
    //std::string input;
    //std::cin >> input;
    std::promise<void> start_task;
    signalr::hub_connection connection = signalr::hub_connection_builder::create("https://rs.jeyunvn.com/voice")
        .with_logging(std::make_shared<logger>(), signalr::trace_level::verbose)
        .build();
    connection.on("Receive Msg:", [](const signalr::value& m)
        {
            std::cout<<"_______________________________" << m.as_array()[0].as_string() << std::endl;
        });
    connection.start([&start_task](std::exception_ptr exception){
        start_task.set_value();
        });

    start_task.get_future().get();
    std::cout << "------------\n";
    signalr::connection_state a = connection.get_connection_state();
    switch (a) {
    case signalr::connection_state::connected:
        std::cout << "connected\n";
        break;
    case signalr::connection_state::connecting:
        std::cout << "connecting\n";
        break;
    case signalr::connection_state::disconnected:
        std::cout << "disconnected\n";
        break;
    case signalr::connection_state::disconnecting:
        std::cout << "disconnecting\n";
        break;

    }

    std::promise<void> invoke_task;
    std::vector<signalr::value> arguments { {
            "topicId: test",
                "userId: uuidv4()",
                "username: test"
        } };
    connection.invoke("MethodName", arguments, [&invoke_task](const signalr::value& result, std::exception_ptr exception) {
        if (exception) {
            std::cout << "Invoke failed \n";
        }
        else {
            // Xử lý kết quả trả về từ lời gọi invoke thành công
            std::cout << "Invoke result: " << result.as_string() << std::endl;
        }

        invoke_task.set_value();
        });

    invoke_task.get_future().get();
    std::vector<signalr::value> msg { "PM Hieu" };
    connection.send("Send Msg", msg);
   

    return 0;
}