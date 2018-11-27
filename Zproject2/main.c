#include "head.h"

#define ser_port 2525
#define BUFF_SIZE 1024

int Connect_server();       //连接服务器
int Search(char make_name[],int op);  //判断用户名或者密码是否存在
                //make_name表示（用户名或密码），                  //op可为1,2分别对应服务器端该判断用户名，判断密码是否存在的操作标志
int User_registration();  //用户注册登录
int Change_password();    //更改密码
int Change_password_2(char change_name[], char old_passwd[]);
     //实现Change_password()中判断用户名密码后修改密码部分
int Login();            //用户登录
int User_login();   //登录
int Sign_out();           //退出

int Online_number();   //在 线 人 数 查 询
int Online_people();   //在 线 人 员 查 询
int Deal_private_message(int f);//私聊  f=2
int Deal_group_message(int f);  //实现群聊功能 f=3
int Group_owner(int f);   //群主f=3
int Deal_message(int f);  //处理信息  f=2 私聊/f=3群聊 /f=4文件
int Shut_up(int index);     //设置0禁言/1解禁/-1踢人
int Group_numbers(int f);   //群成员 f=3
int File_transfer(int f);     //文件传输  f = 4
int File_receive();   //文件接收
int Close_login();      ///退出或者注销时 离线状态

int socket_fd;
struct sockaddr_in ser_addr;

char user_name[20];
char send_user_name[20];  //私聊的对象
char Transport_name[20];  //传输的对象
char filename[20];
char file_name[20];

int Connect_server()    //连接服务器
{
    socket_fd = socket(AF_INET,SOCK_STREAM,0);
    if( -1 == socket_fd )
    {
        perror(" create socket error \n");
        exit(-1);
    }
    char argv[20] = "127.0.0.1";
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(ser_port);
    inet_pton(AF_INET,argv,&ser_addr.sin_addr);

    memset(ser_addr.sin_zero, 0 , 8);

    if( ( connect (socket_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr) ) ) < 0)
    {
        perror(" connet error \n");
        exit(-1);
    }
    return 0;
}

int Online_number()  //在 线 人 数 查 询
{
    char buffer[20] = "8";
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    printf("[Online number]\n");
    char ser_temp[BUFF_SIZE];
    bzero(ser_temp,sizeof(ser_temp));
    int ser_size = recv(socket_fd,ser_temp, sizeof( ser_temp), 0);
    if( ser_size > 0 )
    {
        if(ser_size > sizeof( ser_temp) )
        {
            ser_size = sizeof(ser_temp);
        }

        printf("%s\n", ser_temp);
    }
    else
    {
        perror("recv ser temp error \n");
        exit(-1);
    }
    return 0;
}

int Online_people()   //在 线 人 员 查 询
{
    char buffer[20] = "7";
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    printf("[Online people]\n");

    char ser_temp[BUFF_SIZE];
    bzero(ser_temp,sizeof(ser_temp));
    int ser_size = recv(socket_fd,ser_temp, sizeof( ser_temp), 0);
    if( ser_size > 0 )
    {

        if(ser_size > sizeof( ser_temp) )
        {
            ser_size = sizeof(ser_temp);
        }
        printf("%s\n", ser_temp);
    }
    else
    {
        perror("recv ser temp error \n");
        exit(-1);
    }

    return 0;
}

int Deal_private_message(int f)     //私聊  f=2
{
        bzero(send_user_name,sizeof(send_user_name));
        printf("你要发送的对象\n");
        scanf("%s",send_user_name);
        getchar();
        printf("\n__________________________________\n");
        printf("##输入你要发送的内容,exit结束会话\n\n");
        Deal_message(f);
        return 0;
}


int Deal_group_message(int f)       //实现群聊功能 f=3
{
    char buffer[20];
    sprintf(buffer,"%s9",user_name);
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    char temp[BUFF_SIZE];
    int recv_size = recv( socket_fd, temp, BUFF_SIZE, 0);
    if( recv_size > 0 )
    {
        if( recv_size > BUFF_SIZE )
        {
            recv_size = BUFF_SIZE;
        }
        if( strcmp( temp, "0" ) == 0 )
        {
            Group_owner(f);

        }
        else
        {
            Group_numbers(f);
        }
    }

}

int Group_owner(int f)   //群主 f=3
{
    printf("你是此群的群主\n");
    while(1)
    {
        printf("____________________\n\n");
        printf("    1. 踢    人\n\n");
        printf("    2. 禁    言\n\n");
        printf("    3. 解    禁\n\n");
        printf("    4. 聊    天\n\n");
        printf("    5. 退    出\n\n");
        int num;
        printf("Input a number:");
        scanf("%d",&num);
        getchar();
        switch(num)
        {
            case 1: Shut_up(-1); break;
            case 2: Shut_up(0); break;
            case 3: Shut_up(1); break;
            case 4:
            {
                printf("##你可以发送信息,exit结束\n\n");
                Deal_message(f);
                break;
            }
            case 5:return 0;
        }
    }

}

int Shut_up(int index)     //设置0禁言/1解禁/-1踢人
{
    char buffer[20];
    if( index == 0 )
    {
        printf("输入你要禁言的用户:\n");
        char buff_jin_username[20];
        scanf("%s",buff_jin_username);
        sprintf(buffer,"%sB",buff_jin_username);
    }
    else if( index == 1)
    {
        printf("输入你要解禁的用户:\n");
        char buff_jie_username[20];
        scanf("%s",buff_jie_username);
        sprintf(buffer,"%sC",buff_jie_username);
    }
    else
    {
        printf("输入你要移出群的用户:\n");
        char buff_username[20];
        scanf("%s",buff_username);
        sprintf(buffer,"%sD",buff_username);
    }
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    char temp[BUFF_SIZE];
    int recv_size = recv( socket_fd, temp, BUFF_SIZE, 0);
    if( recv_size > 0 )
    {
        if( recv_size > BUFF_SIZE )
        {
            recv_size = BUFF_SIZE;
        }
        printf("%s\n",temp );
    }
    return 0;
}

int Group_numbers(int f)    //群成员  f=3
{
    char buffer[20];
    sprintf(buffer,"%sA",user_name);
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    char temp[BUFF_SIZE];
    int recv_size = recv( socket_fd, temp, BUFF_SIZE, 0);
    if( recv_size > 0 )
    {
        if( recv_size > BUFF_SIZE )
        {
            recv_size = BUFF_SIZE;
        }
        if( strcmp( temp, "1" ) == 0 )
        {
            printf("\n\n__________________________________\n\n");
            printf("##你可以发送信息,exit结束\n\n");
            Deal_message(f);
        }
        else if( strcmp( temp, "0" ) == 0)
        {
            printf("\n\n__________________________________\n\n");
            printf("很抱歉，你没有发言的权利,你发送的消息讲不会被接收\n\n");
            Deal_message(f);
        }
        else
        {
            printf("你已被移出此群\n\n");
        }
    }
    return 0;
}

int File_transfer(int f)  //文件传输
{


    printf("输入你要传输的对象\n");
    scanf("%s",Transport_name);
    getchar();
    printf("_________________________________\n");
    printf("输入你要传输的文件名,exit结束\n\n");
    Deal_message(f);
    printf("等待信号...\n");
    char temp[20];
    int x = recv(socket_fd,temp,sizeof(temp),0);  //从接收端收信号
    if( strcmp( temp, "yes") == 0 )
    {
        char buffer[900];
        char tt[1000];
        printf("ok\n");
        FILE *fp;
        fp = fopen(filename, "r");
        if( fp == NULL )
        {
            perror("open file error\n");
        }

        while( !feof(fp) )
        {
            bzero(buffer, sizeof(buffer));
            bzero(tt,sizeof(tt));
            fread(buffer, 1, sizeof(buffer), fp);
            usleep(100);
            sprintf(tt,"%s$%sH",buffer, Transport_name);
            int x = send(socket_fd, tt, sizeof(tt), 0);
            if( x < 0 )
            {
                perror("send file error\n");
            }
        //    else
        //    {
        //
          //  }
        }
        printf("[#send file success#]\n");
        fclose(fp);
    }
    else
    {
        return 0;
    }
    return 0;
}

int File_receive()  //文件接收
{
    char flag[20];
    printf("是否接收信息yes/no:");
    scanf("%s",flag);
    getchar();
    if( send(socket_fd, flag, sizeof(flag), 0 ) < 0)
    {
        perror("send File_receive error \n");
    }
    else
    {
        printf("ok\n");
        Deal_message(2);
    }
    return 0;

}


int Deal_message(int f)     //处理信息  f=2 私聊/f=3群聊 /f=4文件
{
    fd_set client_set;
    struct timeval tv;

    char recv_msg[BUFF_SIZE + 100];
    char send_msg[BUFF_SIZE + 100];
    while(1)
    {
        tv.tv_sec = 20;
        tv.tv_usec = 0;
        FD_ZERO(&client_set);
        FD_SET(STDIN_FILENO, &client_set);      //将键盘的输入信息（用户命令）流加入到client_set中
        FD_SET(socket_fd,&client_set);          //将连接主机的套接字（服务器响应）加入到client_set中

        //当client_set中有描述符被改变时，则select立即返回，现在是空转忙等待
        int ret = select( socket_fd + 1, &client_set, NULL, NULL, &tv);
        if( ret < 0 )
        {
            perror("select error \n");
            continue;
        }
        else if( ret == 0 )
        {
            continue;
        }
        else
        {
            if( FD_ISSET(STDIN_FILENO, &client_set))      //检测是否有键盘输入
            {
                char temp[BUFF_SIZE];
                bzero( temp , BUFF_SIZE);
                bzero( send_msg , BUFF_SIZE + 100);
                fgets(temp, BUFF_SIZE, stdin);
                temp[strlen(temp) - 1] = '\0';      //用fgets读最后是\n\0
                printf("\n");
                if( strcmp( temp, "exit") == 0)
                {
                    return 0;
                }
                if( f == 2 )
                {
                    sprintf(send_msg, "%s:%s$%s@",user_name,temp,send_user_name);
                    if( send( socket_fd, send_msg,BUFF_SIZE, 0) < 0)
                    {
                        perror("send error \n");
                    }
                    char temp_s[20];
                    recv(socket_fd, temp_s,sizeof(temp_s), 0 );
                    printf("%s\n\n", temp_s);
                }
                else if( f == 3 )
                {
                    sprintf(send_msg, "%s:%s#",user_name,temp);
                    if( send( socket_fd, send_msg,BUFF_SIZE, 0) < 0)
                    {
                        perror("send error \n");
                    }
                }
                else if( f == 4)
                {
                    strcpy(filename,temp);
                    sprintf(send_msg,"%s:%s&%s&",user_name,temp,Transport_name);
                    if( send( socket_fd, send_msg,BUFF_SIZE, 0) < 0)
                    {
                        perror("send error \n");
                    }
                }

            }
            if( FD_ISSET(socket_fd, &client_set))    //检测sockfd是否变化
            {
                bzero( recv_msg, BUFF_SIZE);
                int recv_size = recv( socket_fd, recv_msg,BUFF_SIZE, 0);
                if( recv_size > 0 )
                {
                    if( recv_size > BUFF_SIZE )
                    {
                        recv_size = BUFF_SIZE;
                    }
                    if( recv_msg[strlen(recv_msg) - 1] == '#')
                    {
                        recv_msg[strlen(recv_msg) - 1] = '\0';
                        printf("[group chat message]~%s\n\n",recv_msg);
                    }
                    else if( recv_msg[strlen(recv_msg) - 1] == '$')
                    {
                        recv_msg[strlen(recv_msg) - 1] = '\0';
                        printf("[private chat message]~%s\n\n",recv_msg);
                    }
                    else if( recv_msg[strlen(recv_msg) - 1] == '&')
                    {
                        recv_msg[strlen(recv_msg) - 1] = '\0';
                        printf("[file]~%s\n\n",recv_msg);
                        strcpy( file_name,recv_msg);
                        int i;
                        for( i = 0; file_name[i]; i++ )
                        {
                            if( file_name[i] == ':')
                            {
                                file_name[i] = '_';
                            }
                        }
                    }
                    else if( recv_msg[strlen(recv_msg) - 1] == 'H' )
                    {
                        recv_msg[strlen(recv_msg) - 1] = '\0';
                    //    printf("[file content]~%s\n\n",recv_msg);
                        printf("[recv file content success]\n");
                        FILE *fp;
                        fp = fopen( file_name, "a+");
                        if( fp == NULL )
                        {
                            perror("open file error\n");
                        }
                        fprintf(fp, "%s",recv_msg );
                        fclose(fp);
                    }

                }
                else if( recv_size < 0 )
                {
                    perror("\n\nrecv message error! \n");
                    exit(-1);
                }
                else
                {
                    printf("\nYou are offline!\n");
                    exit(0);
                }

            }

        }

    }
}

int Search(char make_name[],int op)     //判断用户名或者密码是否存在
                //make_name表示（用户名或密码），                  //op可为1,2分别对应服务器端该判断用户名，判断密码是否存在的操作标志
{
    char send_temp[25];
    bzero(&send_temp,sizeof(send_temp));
    sprintf(send_temp,"%s%d",make_name,op);

    if( send(socket_fd, send_temp, sizeof(send_temp), 0)  < 0)
    {
        perror("send user message error \n");
    }

    char ser_temp[20];
    bzero(&ser_temp,sizeof(ser_temp));
    int ser_size = recv(socket_fd,ser_temp, sizeof( ser_temp), 0);
    if( ser_size > 0 )
    {
        if(ser_size > sizeof( ser_temp) )
        {
            ser_size = sizeof(ser_temp);
        }
        if( strcmp( ser_temp, "0") == 0 )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }
    else
    {
        perror("recv ser temp error \n");
        exit(-1);
   }
   return 0;
}


int Close_login()  //退出或者注销时 离线状态
{
    char buffer[100];
    sprintf(buffer,"%s6",user_name);
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    return 0;
}

int User_registration()     //用户注册
{
    printf("\n\n        用  户  注  册  登  录          \n");
    printf("______________________________________\n\n");
    char ch;   //按n重新输入，按q退出
    int flag = 0;
    char registration_name[20];
    char registration_passwd[20];
    while(1)
    {
        printf("\n输入你要注册的用户名:");
        scanf("%s",registration_name);
        flag = Search(registration_name,1);
        if( flag == 1 )
        {
            printf("\n[#用户名存在，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }
    while(1)
    {
        printf("\n\n设置密码（不超过20位的密码）:");
        scanf("%s",registration_passwd);
        char temp[20];
        printf("\n\n再次输入，确认密码:");
        scanf("%s",temp);
        if( strcmp( registration_passwd, temp ) != 0 )
        {
            printf("\n\n[#两次输入的密码不匹配，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                exit(0);
            }
        }
        else
        {
               break;
        }
    }
    printf("\n用户名:%s\n密码:%s\n\n",registration_name,registration_passwd);
    char buffer[100];
    sprintf(buffer, "insert into user values ('%s', '%s', '%s','%s','%s');3", registration_name,registration_passwd,"-1","YES","0");
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    printf("注册成功！\n");
    return 0;
}



int Change_password()     //更改密码
{
    printf("\n\n        用  户  密  码  更  改          \n");
    printf("______________________________________\n\n");
    char change_name[30];
    char old_passwd[20];

    char ch;
    int flag = 0;
    while(1)
    {
        flag = 0;
        printf("输入需要更改密码的用户名:");
        scanf("%s",change_name);
        flag = Search(change_name,1);
        if( flag == 0 )
        {
            printf("\n[#用户名不存在，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }
    while(1)
    {
        flag = 0;
        printf("\n输入原来的密码:");
        scanf("%s",old_passwd);
        flag = Search(old_passwd,2);
        if( flag == 0 )
        {
            printf("\n[#密码错误，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }
    Change_password_2(change_name, old_passwd);
    printf("密码更改成功!\n");
    return 0;
}

int Change_password_2(char change_name[] , char old_passwd[])
//实现Change_password()中判断用户名密码后修改密码部分
{
    char ch;
    while(1)
    {
        char new_passwd[20];
        printf("\n输入新密码:");
        scanf("%s",new_passwd);
        char agin_passwd[20];
        printf("\n确认输入的密码:");
        scanf("%s",agin_passwd);
        if( strcmp( new_passwd, agin_passwd) == 0 )
        {
            char buffer[100];
            sprintf(buffer, "update user set password = '%s' where password = '%s' and name = '%s';4" , new_passwd, old_passwd,change_name);
            if( send(socket_fd, buffer, sizeof(buffer), 0)  < 0)
            {
                perror("send user password error \n");
                exit(-1);
            }
            else
            {
                return 0;
            }
        }
        else
        {
            printf("[#两次输入不匹配按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
    }
    return 0;
}

int Login()         //用户登录
{
    int f = 1;
    while(f)
    {
        f = User_login();
        if( f == 0 )
        {
            break;
        }
    }
    return 0;
}

int User_login()      //登录
{
    printf("\n\n         用    户    登    录          \n");
    printf("______________________________________\n\n");
    char ch;  //按n重新输入，按q退出
    char send_temp[25];
    char ser_temp[20];
    int flag;
    while(1)   //判断用户名是否存在
    {
        printf("\n输入你的用户名:");
        scanf("%s",user_name);
        flag = Search(user_name,1);
        if( flag == 0 )
        {
            printf("\n[#用户名不存在，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }
    char user_passwd[20];
    while(1)   //判断密码是否正确
    {
        printf("\n请输入密码:");
        scanf("%s",user_passwd);

        flag = Search(user_passwd,2);
        if( flag == 0 )
        {
            printf("\n[#密码错误，按任意键重新输入，按q退出#]\n");
            getchar();
            scanf("%c",&ch);
            if( ch == 'q')
            {
                return 0;
            }
        }
        else
        {
            break;
        }
    }

    printf("\n登录成功！\n");
    char buffer[100];
    sprintf(buffer,"%s5",user_name);
    if( send( socket_fd, buffer, sizeof( buffer) , 0) < 0)
    {
        perror(" send registration user message error \n");
        exit(-1);
    }
    while(1)
    {
        printf("_____________________________\n\n");
        printf("    0.  在 线 人 数 查 询          \n\n");
        printf("    1.  在 线 人 员 查 询          \n\n");
        printf("    2.  私       聊          \n\n");
        printf("    3.  群       聊                 \n\n");
        printf("    4.  注       销           \n\n");
        printf("    5.  文 件 传 输\n\n");
        printf("    6.  文 件 接 收\n\n");
        printf("    7.  退       出\n\n");

        fd_set set;
        struct timeval tv;
        char recv_msg[BUFF_SIZE + 100];
        while(1)
        {
            tv.tv_sec = 20;
            tv.tv_usec = 0;
            FD_ZERO(&set);
            FD_SET(STDIN_FILENO, &set);      //将键盘的输入信息（用户命令）流加入到client_set中
            FD_SET(socket_fd,&set);          //将连接主机的套接字（服务器响应）加入到client_set中

            //当client_set中有描述符被改变时，则select立即返回，现在是空转忙等待
            int ret = select( socket_fd + 1, &set, NULL, NULL, &tv);
             if( ret < 0 )
            {
                perror("select error \n");
                continue;
            }
            else if( ret == 0 )
            {
                continue;
            }
            else
            {
                if( FD_ISSET(socket_fd, &set))    //检测sockfd是否变化
                {
                    bzero( recv_msg, BUFF_SIZE);
                    int recv_size = recv( socket_fd, recv_msg,BUFF_SIZE, 0);
                    if( recv_size > 0 )
                    {
                        if( recv_size > BUFF_SIZE )
                        {
                            recv_size = BUFF_SIZE;
                        }
                        if( recv_msg[strlen(recv_msg) - 1] == '#')
                        {
                            recv_msg[strlen(recv_msg) - 1] = '\0';
                            printf("[group chat message]~%s\n\n",recv_msg);
                        }
                        else if( recv_msg[strlen(recv_msg) - 1] == '$')
                        {
                            recv_msg[strlen(recv_msg) - 1] = '\0';
                            printf("[private chat message]~%s\n\n",recv_msg);
                        }
                        else if( recv_msg[strlen(recv_msg) - 1] == '&')
                        {
                            recv_msg[strlen(recv_msg) - 1] = '\0';
                            printf("[file]~%s\n\n",recv_msg);
                            strcpy( file_name,recv_msg);
                            int i;
                            for( i = 0; file_name[i]; i++ )
                            {
                                if( file_name[i] == ':')
                                {
                                    file_name[i] = '_';
                                }
                            }
                        }
                    }
                }
                if( FD_ISSET(STDIN_FILENO, &set))     //检测是否有键盘输入
                {
                    break;
                }
            }


        }
        int num;
        printf("Input a op_number:");
        scanf("%d",&num);
        getchar();
        switch(num)
        {
              case 0: Online_number();  break;
              case 1: Online_people();  break;
              case 2: Deal_private_message(2); break;
              case 3: Deal_group_message(3); break;
              case 4: Close_login(); return 1;
              case 5: File_transfer(4); break;
              case 6: File_receive(); break;
              case 7: Close_login(); return 0;
        }

    }

    return 0;

}
int Sign_out()
{
    exit(0);
}



int main(void)
{
    Connect_server();
    while(1)
    {
        system("date");
        printf("\n       Welcome to chat software!    \n");
        printf("______________________________________\n\n");
        printf("    1.  用  户  注  册  登  录 \n\n");
        printf("    2.  用  户  密  码  更  改 \n\n");
        printf("    3.  用  户  登  录 \n\n");
        printf("    4.  用  户  退  出  程  序\n\n");

        printf("Input a menu_number:");
        int menu_num;
        scanf("%d",&menu_num);
        switch(menu_num)
        {
              case 1: User_registration(); break;
              case 2: Change_password(); break;
              case 3: Login(); break;
              case 4: Sign_out(); break;
        }
    }

    return 0;
}