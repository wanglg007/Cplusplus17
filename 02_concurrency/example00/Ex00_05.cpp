//(0.5)使用分离线程去处理其他文档
#include <thread>
#include <string>

void open_document_and_display_gui(std::string const &filename) {

}

bool done_editing() {
    return true;
}

enum commond_type {
    open_new_document
};

struct user_command {
    commond_type type;

    user_command() : type(open_new_document) {}
};

user_command get_user_input(){
    return user_command();
}

std::string get_filename_from_user(){
    return "foo.doc";
}

void process_user_input(user_command const& cmd){

}

void edit_document(std::string const& filename){
    open_document_and_display_gui(filename);
    while(!done_editing()){
        user_command cmd = get_user_input();
        if(cmd.type == open_new_document){
            std::string const new_name = get_filename_from_user();
            std::thread t(edit_document,new_name);          //(1)
            t.detach();                                     //(2)
        }else{
            process_user_input(cmd);
        }
    }
}

int main(){
    edit_document("bar.doc");
}

/**
 * 使用detach()会让线程在后台运行，这意味着主线程不能与之产生直接交互，即不会等待这个线程结束。
 *
 * 如果用户选择打开一个新文档，为了迅速打开文档，需要启动一个新线程去打开新文档①，并分离线程②。与当前线程做出的操作一样，新线程只不过是打开另一个
 * 文件而已。所以edit_document函数可以复用通过传参的形式打开新的文件。
 *
 */