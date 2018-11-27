create database user_message character set gbk;

use user_message;

create table user (
        name		  char(30) not null,
        password      char(20) not null,
        telent        char(10) not null,
        group_chat    char(10) not null,
        flag          char(10) not null
);







