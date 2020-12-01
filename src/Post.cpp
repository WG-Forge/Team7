#include "Post.h"

int Post::idx(){
    return idx_;
}

QString Post::name(){
    return name_;
}

enum PostType Post::type(){
    return type_;
}

std::vector<Event*>& Post::events(){
    return events_;
}


