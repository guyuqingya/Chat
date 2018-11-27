#include "head.h"
#include <mysql.h>

#define ser_port 2525
#define CLI_NUM 1000
#define BUFF_SIZE 1124



void Get_mysql();           //连接数据库
int Serach_mysql( char user[], int x );  //查找数据库,x=0 表示查找用户名 x=1表示查找密码
int Insert_mysql( int i);           //向数据库添加信息
int Change_mysql( int i ); //改变数据库的密码信息
int Close_mysql();

int Judge(int i,int x);  //判断用户名是否存在,密码是否正确
                    //x=0 表示判断用户名 x=1表示判断密码
int Judge_qunzhu(int i);    //判断是否存在群主
int Judge_group_stat(int i);   //判断在群里的状态（可以发言，不能发言，被踢了）
int Shut_up(int i,int f);   //f = 0禁言， f = 1解禁 f = -1踢人
int Private_chat(int i);    //处理私聊信息
int Group_chat(int i);      //处理群聊信息
int  Send_group_message(int i);  //将信息发送给其他群成员
int Change_stat(int f,int i);  //改变是否在线的状态 f=1在线  f=0离线
int File_name(int i); //文件名传输
int File_content(int i); //文件内容传输

int Online_people(int i);   //在 线 人 员 查 询
int Online_number(int i);   //在 线 人 数 查 询

MYSQL mysql;  //一个数据库链接指针
MYSQL_RES* res; //一个结果集结构体
MYSQL_ROW row;  //char** 二维数组，存放一条条记录

int client_fds[CLI_NUM];
char user_name[CLI_NUM][20];

char send_message[BUFF_SIZE];
char recv_message[BUFF_SIZE];

void Get_mysql()    //连接数据库
{
    if ( ( mysql_init(&mysql) ) == NULL )  //初始化数据库
    {
        perror("mysql init error \n");
        exit(-1);
    }

    mysql_options(&mysql, MYSQL_SET_CHARSET_NAME, "gbk");   //设置编码方式

     //连接数据库
    if ( ( mysql_real_connect(&mysql,"localhost","root","","user_message",0,NULL,0) ) == NULL )
    {
        perror("mysql connect error \n");
        exit(-1);
    }

}

int Serach_mysql( char user[], int x )  //查找数据库,x=0 表示查找用户名 x=1表示查找密码
{
    int flag = 0;
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[x], user) == 0 )
        {
            return 1;
        }
    }
    return 0;
}

int Insert_mysql( int i )   //向数据库添加信息
{
    mysql_set_character_set(&mysql, "utf8");
    recv_message[strlen(recv_message) - 1] = '\0';
    printf("即将讲此用户信息加载入数据库[%s]\n",recv_message);
    mysql_real_query(&mysql, recv_message,strlen(recv_message));
    return 0;
}

int Change_mysql( int i )   //改变数据库的密码信息
{
    mysql_set_character_set(&mysql,"utf8");
    recv_message[strlen(recv_message) - 1] = '\0';
    printf("即将讲此用户信息 更改 入数据库[%s]\n",recv_message);
    if( mysql_real_query(&mysql, recv_message,strlen(recv_message)) )
    {
        perror("update error\n");
    }

    return 0;
}

int Close_mysql()
{
        mysql_free_result(res);
        mysql_close(&mysql);
}

int Judge_qunzhu(int i)   //判断是否存在群主
{
    recv_message[strlen(recv_message) - 1] = '\0';
    int flag = 0;
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[4],"1" ) == 0 )
        {
            flag = 1;
            break;
        }
    }
    char temp[20];
    if( flag == 0 )
    {
        strcpy( temp, "0");
        mysql_set_character_set(&mysql,"utf8");
        char buffer[100];
        sprintf(buffer, "update user set flag = '%s' where name = '%s';" ,"1", recv_message);
        if( mysql_real_query(&mysql, buffer,strlen(buffer)) )
        {
            perror("update error\n");
        }
    }
    else
    {
        if(strcmp( row[0], recv_message) == 0)
        {
            strcpy( temp, "0");
        }
        else
        {
            strcpy( temp, "1");
        }
    }
    send( client_fds[i], temp, sizeof(temp),0);
}

int Judge_group_stat(int i)  //判断在群里的状态（可以发言，不能发言，被踢了）
{
    recv_message[strlen(recv_message) - 1] = '\0';
    int flag = 0;
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[0],recv_message ) == 0 )
        {
            if( strcmp( row[3], "YES") == 0 )
            {
                flag = 1;
                break;
            }
            else if( strcmp( row[3],"NULL") == 0)
            {
                flag = -1;
                break;
            }

        }
    }
    char temp[20];
    if( flag == 0 )
    {
        strcpy( temp,"0");
    }
    else if( flag == 1)
    {
        strcpy( temp, "1");
    }
    else
    {
        strcpy( temp, "-1");
    }
    send( client_fds[i], temp, sizeof(temp),0);
}

int Shut_up(int i,int f)    //f = 0禁言， f = 1解禁  f = -1踢人
{
    mysql_set_character_set(&mysql,"utf8");
    recv_message[strlen(recv_message) - 1] = '\0';
    char buffer[100];
    if( f == 1 )
    {

        sprintf(buffer, "update user set  group_chat= '%s' where name = '%s';", "YES",recv_message);
    }
    else if( f == 0 )
    {
        sprintf(buffer, "update user set group_chat = '%s' where name = '%s';" ,"NO",  recv_message);
    }
    else
    {
        sprintf(buffer, "update user set group_chat = '%s' where name = '%s';" ,"NULL",  recv_message);
    }
    if( mysql_real_query(&mysql, buffer,strlen(buffer)) )
    {
        char temp[20]="[#faliue#]";
        send(client_fds[i],temp, sizeof(temp), 0 );
        perror("update error\n");
    }
    else
    {
        char temp[20]="[#success#]";
        send(client_fds[i],temp, sizeof(temp), 0 );
    }
}

int Private_chat(int i)     //处理私聊信息
{
    char send_user_name[20];
    int k = 0;
    recv_message[strlen(recv_message) - 1] = '\0';
    int len = strlen(recv_message) - 1;
    int x;
    for( x = len; x >= 0; x-- )
    {
        if( recv_message[x] != '$' )
        {
            send_user_name[k++] = recv_message[x];
        }
        else
        {
            break;
        }
    }
    recv_message[x + 1] = '\0';
    send_user_name[k] = '\0';
    for( int i = 0, j = k - 1; i < j; i++, j-- )
    {
        char temp = send_user_name[i];
        send_user_name[i] = send_user_name[j];
        send_user_name[j] = temp;
    }
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[0], send_user_name) == 0 )
        {
            if( strcmp(row[2], "-1") != 0  )
            {
                if(send(atoi(row[2]), recv_message,BUFF_SIZE,0) < 0)
                {
                    perror("send error\n");
                    char temp[20]="[#faliue1#]";
                    send(client_fds[i],temp, sizeof(temp), 0 );
                }
                else
                {
                    char temp[20]="[#success#]";
                    send(client_fds[i],temp, sizeof(temp), 0 );
                }
            }
            else
            {
                char temp[20]="[#faliue#2]";
                send(client_fds[i],temp, sizeof(temp), 0 );
            }
        }
    }

}

int Group_chat(int i)       //处理群聊信息
{
    char username[20];
    int k;
    for( k = 0; recv_message[k]; k++ )
    {

        if(recv_message[k] != ':')
        {
            username[k] = recv_message[k];

        }
        else
        {
            break;
        }
    }
    username[k] = '\0';
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[0], username) == 0 )
        {
            if( strcmp( row[3], "YES") == 0 )
            {
                Send_group_message(i);
            }
        }
    }
    return 0;
}

int  Send_group_message(int i)  ////将信息发送给其他群成员
{
    MYSQL_RES* buffer; //一个结果集结构体
    MYSQL_ROW temp;
    mysql_query(&mysql, "select * from user");
    buffer = mysql_store_result(&mysql);
    while( temp = mysql_fetch_row(buffer))
    {
        if( strcmp( temp[2], "-1") != 0  && strcmp( temp[3], "NULL") != 0)
        {
            if( atoi(temp[2]) != client_fds[i])
            {
                if( send( atoi(temp[2]), recv_message,BUFF_SIZE,0) < 0 )
                {
                    perror("send group message error \n");
                    return 0;
                }
            }
        }
    }
}

int Judge(int i, int x)        //判断用户名是否存在,密码是否正确
                    //x=0 表示判断用户名 x=1表示判断密码
{
    recv_message[strlen(recv_message) - 1] = '\0';
    int flag = Serach_mysql(recv_message, x );
    char temp[20];
    if( flag == 0 )
    {
        strcpy( temp,"0");
    }
    else
    {
        strcpy( temp, "1");
    }
    send( client_fds[i], temp, sizeof(temp),0);
    return 0;
}

int Change_stat(int f,int i)  //改变是否在线的状态 f=1在线  f=0离线
{
    mysql_set_character_set(&mysql,"utf8");
    recv_message[strlen(recv_message) - 1] = '\0';
    char buffer[100];
    if( f == 1 )
    {

        sprintf(buffer, "update user set telent = '%d' where name = '%s';" , client_fds[i], recv_message);
    }
    else
    {
        sprintf(buffer, "update user set telent = '%s' where name = '%s';" ,"-1",  recv_message);
    }

    if( mysql_real_query(&mysql, buffer,strlen(buffer)) )
    {
        perror("update error\n");
    }
}

int File_name(int i )  //文件名
{
    char send_user_name[20];
    int k = 0;
    recv_message[strlen(recv_message) - 1] = '\0';
    int len = strlen(recv_message) - 1;
    int x;
    for( x = len; x >= 0; x-- )
    {
        if( recv_message[x] != '&' )
        {
            send_user_name[k++] = recv_message[x];
        }
        else
        {
            break;
        }
    }
    recv_message[x + 1] = '\0';
    send_user_name[k] = '\0';
    for( int i = 0, j = k - 1; i < j; i++, j-- )
    {
        char temp = send_user_name[i];
        send_user_name[i] = send_user_name[j];
        send_user_name[j] = temp;
    }
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[0], send_user_name) == 0 )
        {
            if( strcmp(row[2], "-1") != 0 )
            {
                send(atoi(row[2]), recv_message,BUFF_SIZE,0);  //发送信号给接收端
                char temp[20];
                int x = recv(atoi(row[2]),temp,sizeof(temp),0);  //从接收端收信号
                if( x > 0 )
                {
                    send(client_fds[i],temp, sizeof(temp), 0 ); //将信号发送给发送端
                }
            }
            else
            {
                char temp[20]="[#faliue#]";
                send(client_fds[i],temp, sizeof(temp), 0 );
            }
        }
        else
        {
            char temp[20]="[#faliue#]";
            send(client_fds[i],temp, sizeof(temp), 0 );
        }
    }
}

int File_content(int i)
{
    char send_user_name[20];
    int k = 0;
    recv_message[strlen(recv_message) - 1] = '\0';
    int len = strlen(recv_message) - 1;
    int x;
    for( x = len; x >= 0; x-- )
    {
        if( recv_message[x] != '$' )
        {
            send_user_name[k++] = recv_message[x];
        }
        else
        {
            break;
        }
    }
    recv_message[x] = 'H';
    recv_message[x + 1] = '\0';
    send_user_name[k] = '\0';
    for( int i = 0, j = k - 1; i < j; i++, j-- )
    {
        char temp = send_user_name[i];
        send_user_name[i] = send_user_name[j];
        send_user_name[j] = temp;
    }
    mysql_query(&mysql, "select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[0], send_user_name) == 0 )
        {
            if( strcmp(row[2], "-1") != 0 )
            {
                send(atoi(row[2]), recv_message,BUFF_SIZE,0);  //发送信号给接收端
            }
            else
            {
                char temp[20]="[#faliue#]";
                send(client_fds[i],temp, sizeof(temp), 0 );
            }
        }
        else
        {
            char temp[20]="[#faliue#]";
            send(client_fds[i],temp, sizeof(temp), 0 );
        }
    }
}

int Online_people(int i)   //在 线 人 员 查 询
{
    char buffer[BUFF_SIZE];
    bzero(buffer,BUFF_SIZE);
    mysql_query(&mysql,"select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[2], "-1") != 0 )
        {
            strcat(buffer,row[0]);
            strcat(buffer," ");
        }
    }
    printf("$$%s\n",buffer );
    if( send( client_fds[i], buffer, strlen(buffer), 0 ) < 0 )
    {
        perror("send  Online people error\n");
    }
    return 0;
}

int Online_number(int i)        //在 线 人 数 查 询
{
    int count = 0;
    char buffer[20];
    bzero(buffer,sizeof(buffer));
    mysql_query(&mysql,"select * from user");
    res = mysql_store_result(&mysql);
    while( row = mysql_fetch_row(res))
    {
        if( strcmp(row[2], "-1") != 0 )
        {
            count++;
        }
    }
    int k =0;
    while( count != 0 )
    {
        buffer[k] = (count%10) + '0';
        count = count / 10;
        k++;
    }
    buffer[k] = '\0';
    printf("$$%s\n",buffer);
    for( int p = 0, j = k - 1; p < j; p++, j--)
    {
        char temp = buffer[p];
        buffer[p] = buffer[j];
        buffer[j] = temp;
    }
    printf("##%s\n",buffer);
    send( client_fds[i], buffer, sizeof(buffer), 0 );
    return 0;
}

int main()
{
    Get_mysql();
    struct sockaddr_in ser_addr;
    int ser_socket_fd;

    ser_addr.sin_family = AF_INET;
    ser_addr.sin_port = htons(ser_port);
    ser_addr.sin_addr.s_addr = htonl(INADDR_ANY);

    memset(ser_addr.sin_zero, 0 , 8);

    if( ( ser_socket_fd = socket (AF_INET, SOCK_STREAM, 0 ) ) < 0 )
    {
        perror("socket error \n");
        exit(-1);
    }

    if( ( bind(ser_socket_fd, (struct sockaddr *)&ser_addr, sizeof(ser_addr) ) ) < 0 )
    {
        perror("bind error \n");
        exit(-1);
    }

    if( ( listen( ser_socket_fd, 7 ) ) < 0 )
    {
        perror("listen error \n");
        exit(-1);
    }

    printf("wait for client connect! \n");
    int max_fd = 1;

    fd_set ser_Readset;   //创建文件描述符集合
    struct timeval tv;

    while(1)
    {
        tv.tv_sec = 50;
        tv.tv_usec = 0;
        FD_ZERO(&ser_Readset);
        FD_SET(ser_socket_fd, &ser_Readset);
        //将连接主机的套接字（服务器响应）加入到ser_Readset中
        if( max_fd < ser_socket_fd)
        {
            max_fd = ser_socket_fd;
        }

        for( int i = 0; i < CLI_NUM; i++ )   //从client_fds中取得fd加入ser_Readset
        {

            if( client_fds[i] != 0 )
            {
                FD_SET( client_fds[i], &ser_Readset);
                if( max_fd < client_fds[i])
                {
                    max_fd = client_fds[i];
                }
            }
        }

        //当client_set中有描述符被改变时，则select立即返回，现在是空转忙等待
        int ret = select( max_fd + 1, &ser_Readset, NULL ,NULL , &tv);
        if( ret < 0 )
        {
            perror("select error \n");
            continue;
        }
        else if( ret == 0 )
        {
            printf(" select time out! \n");
            continue;
        }
        else
        {
            if( FD_ISSET(ser_socket_fd, &ser_Readset))         //检测sockfd是否变化
            {
                struct sockaddr_in clinet_addr;
                socklen_t addr_len;
                int client_socket_fd;

                client_socket_fd = accept( ser_socket_fd, (struct sockaddr *)&clinet_addr, &addr_len);
                printf(" new connection client_socket_fd = %d \n", client_socket_fd);

                if( client_socket_fd > 0 )
                {
                    int index = -1;
                    for( int i = 0; i < CLI_NUM; i++ )
                    {
                        if( client_fds[i] == 0 )
                        {
                            index = i;
                            client_fds[i] = client_socket_fd;
                            break;
                        }
                    }
                    if( index >= 0 )
                    {
                        printf(" New client(%d)join success %s:%d \n", index, inet_ntoa(clinet_addr.sin_addr),ntohs(clinet_addr.sin_port));
                    }
                    else
                    {
                        bzero(send_message, BUFF_SIZE);
                        strcpy(send_message, "服务器忙！请稍后再登...\n");
                        send( client_socket_fd, send_message, BUFF_SIZE, 0);
                    }

                }
            }
        }

        for( int i = 0; i < CLI_NUM; i++ )          //检测client_fds中是否有变化的
        {
           // int temp_cli_fd = client_fds[i];

            if( client_fds[i] != 0 )
            {
                if( FD_ISSET(client_fds[i] , &ser_Readset ))
                {
                    bzero( recv_message, BUFF_SIZE);
                    int size_recv = recv( client_fds[i], recv_message,BUFF_SIZE,0);
                    if( size_recv > 0 )
                    {
                        if( size_recv > BUFF_SIZE)
                        {
                            size_recv = BUFF_SIZE;
                        }
                        recv_message[size_recv] = '\0';
                        //接受一个客户端的信息后,
                    //    printf("[#from client%d#]%s\n",i,recv_message);
                        switch(recv_message[strlen(recv_message) - 1])
                        {
                            case '1': Judge(i,0); break;    //判断用户名是否存在
                            case '2': Judge(i,1);break;     //判断密码是否正确
                            case '3': Insert_mysql(i); break; //将信息插入到数据库
                            case '4': Change_mysql(i); break;   //改变数据库的密码信息
                            case '5': Change_stat(1,i); break;   //登录成功，在数据库中将用户状态改为client_fds[i]
                            case '6': Change_stat(0,i); break;   //登录成功，在数据库中将用户状态改为-1
                            case '7': Online_people(i); break;   //在 线 人 员 查 询
                            case '8': Online_number(i); break;
                            case '9': Judge_qunzhu(i); break; //判断是否存在群主
                            case 'A': Judge_group_stat(i); break;   //判断是否可以发言
                            case 'B': Shut_up(i,0); break;   //禁言
                            case 'C': Shut_up(i,1); break;  //解禁
                            case 'D': Shut_up(i,-1); break; //踢人
                            case '&': File_name(i); break;  //文件名传输
                            case 'H': File_content(i); break;  //文件内容传输
                            case '@': Private_chat(i); break; //处理私聊信息
                            case '#': Group_chat(i); break;  //处理群聊信息
                        }
                    }
                    else if( size_recv < 0 )
                    {
                        printf("Error from client(%d).\n",i);
                    }
                    else
                    {
                        FD_CLR(client_fds[i],&ser_Readset);   //将没有事情发生的fd清空
                        client_fds[i] = 0;
                        printf("client(%d)quit\n", i);
                    }
                }
            }
        }

    }
return 0;
}


