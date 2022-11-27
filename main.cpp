#include <dirent.h>
#include <string.h>
#include <iostream>
#include <sys/stat.h>
#include <vector>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include<termios.h>
#include<stdio.h>
#include<sys/ioctl.h>
#include<cstring>
#include<stack>
#include <sys/wait.h>
using namespace std;
using namespace std;

//to store all the details of a particular file in the directory
struct file_details{
    string name;
    string path;
    string permissions;
    bool isdir;
    size_t size;
    string hsize;
    string user;
    string group;
    string mtime;
};

//to store the amount of data from the file vector that is to be displayed according to screen size
struct vector_details{
    int v_start;
    int v_end;
    int factor;

} vd;


// to get the details of all files from the current directory and populate them in the vector
vector <struct file_details> get_file_details( )
{
    char cwd[200];
    char size_ab[] = {'B','K','M','G','T','P'};
    
    getcwd(cwd, sizeof(cwd));
	struct dirent *dirp;
	DIR *dp;
	vector <struct file_details> vfd;
	dp = opendir( cwd );
	while ( ( dirp = readdir ( dp ) ) != NULL )
	{	
        int c_s_p  = 0;
        struct group *gd;
        struct passwd *ud;
        char temp_path[500];
        struct file_details fd;
		struct stat statbuf;
		int dl = strlen( cwd );
        strcpy(temp_path, cwd);
        strcat(temp_path,"/");
        strcat(temp_path, dirp->d_name);
        fd.name.assign(dirp -> d_name, dirp->d_name + strlen(dirp->d_name));
        fd.path.assign(temp_path, temp_path + strlen(temp_path));
        
		stat(  temp_path , &statbuf );

        ud = getpwuid(statbuf.st_uid);
        fd.user.assign(ud->pw_name, strlen(ud->pw_name));
        gd = getgrgid(statbuf.st_gid);
        fd.group.assign(gd->gr_name, strlen(gd->gr_name));

        fd.size = statbuf.st_size;
        long long temp_size = fd.size;
        while( temp_size > 1024)
        {
            temp_size /= 1024;
            c_s_p++;
        }
        fd.hsize = to_string(temp_size) + size_ab[c_s_p];

        struct tm dt = *(gmtime(&statbuf.st_mtime));
        fd.mtime =  to_string(dt.tm_mday) + "-" + to_string(dt.tm_mon) + "-" + to_string(dt.tm_year + 1900) + "  "+ to_string(dt.tm_hour)+":"+ to_string(dt.tm_min)+":"+ to_string(dt.tm_sec);
        if ( statbuf.st_mode & S_IRUSR ) fd.permissions.append("r"); else fd.permissions.append("-");   
        if ( statbuf.st_mode & S_IWUSR ) fd.permissions.append("w"); else fd.permissions.append("-");   
        if ( statbuf.st_mode & S_IXUSR ) fd.permissions.append("x"); else fd.permissions.append("-");   
        fd.permissions.append(" ");
        if ( statbuf.st_mode & S_IRGRP ) fd.permissions.append("r");  else fd.permissions.append("-");    
        if ( statbuf.st_mode & S_IWGRP ) fd.permissions.append("w"); else fd.permissions.append("-");   
        if ( statbuf.st_mode & S_IXGRP ) fd.permissions.append("x"); else fd.permissions.append("-");   
        fd.permissions.append(" ");
        if ( statbuf.st_mode & S_IROTH ) fd.permissions.append("r"); else fd.permissions.append("-");   
        if ( statbuf.st_mode & S_IWOTH ) fd.permissions.append("w"); else fd.permissions.append("-");   
        if ( statbuf.st_mode & S_IXOTH ) fd.permissions.append("x"); else fd.permissions.append("-");   

		if ( S_ISDIR( statbuf.st_mode) == 0 )
                	fd.isdir = 0;
		else 
		{
			fd.isdir = 1;
		}
        vfd.push_back(fd);
	}
	closedir( dp );
    return vfd;
}
		
//stores file details to be printed on the screen in the buffer
string print_st( vector<struct file_details> vfd)
{
    int nl;
    string buffer;
   
    buffer += "--------Name--------  ";
    buffer += "permissions  ";
    buffer += "---User---  ";
    buffer += "--Group---  ";
    buffer += "Size-  ";
    buffer += "-Last-Access-Time--  \n" ;
    //buffer += to_string(buffer.length()+1);
    //cout << "--------Name--------  " << "-permissions  " << "---User---  " << "--Group---  " << "Size-  " << "-Last-Access-Time--  " << endl;
	for ( int i = vd.v_start; i <= vd.v_end; i++)
    {
        nl = vfd[i].name.length();
        if( nl < 20)
        {
            buffer += vfd[i].name;
            //cout << vfd[i].name;
            for (int j = 0; j < 20 - nl; j++)
                buffer += " ";
                //cout << " ";
        }
        else if( nl > 20)
        {
            buffer += vfd[i].name.substr(0,17) + "...";
            //cout << vfd[i].name.substr(0,17) << "...";
        }
        else
        {
            buffer += vfd[i].name;
            //cout << vfd[i].name;
        }
        buffer += "  ";
        if( vfd[i].isdir == 1 )
            buffer += 'd';
        else
            buffer += '-';
        buffer +=  vfd[i].permissions + "  ";
        //cout << "  ";
        //cout << vfd[i].permissions << "  ";

        nl = vfd[i].user.length();
        if( nl < 10)
        {
            buffer += vfd[i].user;
            //cout << vfd[i].user;
            for (int j = 0; j < 10 - nl; j++)
                buffer += " ";
                //cout << " ";
        }
        else if( nl > 10)
        {
            buffer += vfd[i].user.substr(0,7) + "...";
            //cout << vfd[i].user.substr(0,7) << "...";
        }
        else
        {
            buffer += vfd[i].user;
            //cout << vfd[i].user;
        }
        buffer += "  ";
        //cout << "  ";

        nl = vfd[i].group.length();
        if( nl < 10)
        {
            buffer += vfd[i].group;
            //cout << vfd[i].group;
            for (int j = 0; j < 10 - nl; j++)
                buffer += " ";
               // cout << " ";
        }
        else if( nl > 10)
        {
            buffer += vfd[i].group.substr(0,7) + "...";
            //cout << vfd[i].group.substr(0,7) << "...";
        }
        else
        {
            buffer += vfd[i].group;
           // cout << vfd[i].group;
        }
        buffer += "  ";
       // cout << "  ";
        
        nl = vfd[i].hsize.length();
        if( nl < 5)
        {
            buffer += vfd[i].hsize;
            //cout << vfd[i].hsize;
            for (int j = 0; j < 5 - nl; j++)
                buffer += " ";
               // cout << " ";
        }
        else
        {
            buffer += vfd[i].hsize;
           // cout << vfd[i].hsize;
        }
        buffer += "  ";
       // cout << "  ";

        nl = vfd[i].mtime.length();
        if( nl < 20)
        {
            buffer += vfd[i].mtime;
           // cout << vfd[i].mtime;
            for (int j = 0; j < 20 - nl; j++)
                buffer += " ";
               // cout << " ";
        }
        else
        {
            buffer += vfd[i].mtime;
           // cout << vfd[i].mtime;
        }
      //  buffer += to_string(buffer.length()+1);
        buffer += "  \n";
        //cout << "  ";
        //cout << endl;
    }
    return buffer;
	
}

//stores details about the terminal i.e orignal flags, no. of rows and no. of columns
struct w_conf{
    struct termios inp;
    int coln;
    int rown;
    int cur_pos;
    string start_directory;
} wc;

//to enter non-canon mode
void enter_noncanon(){
    tcgetattr(STDIN_FILENO,&(wc.inp));
    struct termios otp = wc.inp;
    otp.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
//inp.c_oflag &= ~(OPOST);
    otp.c_cflag |= (CS8);
    otp.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    otp.c_cc[VMIN] = 0;
    otp.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &otp);
//return otp;
}

//non-canon mode with echo on (for command mode)
void set_echo_on(){
    struct termios otp = wc.inp;
    otp.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
//inp.c_oflag &= ~(OPOST);
    otp.c_cflag |= (CS8);
    otp.c_lflag &= ~( ICANON | IEXTEN | ISIG);
    otp.c_cc[VMIN] = 0;
    otp.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &otp);
//return otp;
}

//to return from command to normal mode
void set_echo_of(){
     struct termios otp = wc.inp;
    otp.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
//inp.c_oflag &= ~(OPOST);
    otp.c_cflag |= (CS8);
    otp.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    otp.c_cc[VMIN] = 0;
    otp.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &otp);
}

//to update the rows and columns on terminal screen
void update_window_size(){
    struct winsize ws;
    ioctl(STDOUT_FILENO, TIOCGWINSZ, &ws);
    wc.coln = ws.ws_col;
    wc.rown = ws.ws_row;
    if(wc.coln >= 89)
        vd.factor = 1;
    else if(wc.coln >= 44)
        vd.factor = 2;
    else
        vd.factor = 3;
}

//to return terminal flags to orignal state
void exit_noncanon(){
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &(wc.inp));
}

// to store file paths for left and right arrow keys
stack <string> s_forward, s_backward;

// l key function
void scroll_down(vector <struct file_details> vfd)
{
    if (vd.v_end < vfd.size() - 1)
        {
            vd.v_start++;
            vd.v_end++;
             if( wc.cur_pos == 2  )
        return;
    else
        wc.cur_pos--;
        }
    
   
}

//k key function
void scroll_up(vector <struct file_details> vfd)
{
    if (vd.v_start > 0)
        {
            vd.v_start--;
            vd.v_end--;
             int max = vd.v_end - vd.v_start + 2;
    if( max > wc.rown - 2  )
        max = wc.rown - 2;
    if( wc.cur_pos == max  )
        return;
    else
        wc.cur_pos++;
        }
    
}

//up arrow key function
void up(vector <struct file_details> vfd){
    if( wc.cur_pos == 2 )
        {
            if( vd.v_start > 0 )
            {
                vd.v_start--;
                vd.v_end--;
            }
        }
    else
        wc.cur_pos--;
}

//down arrow key funvtion
void down(vector <struct file_details> vfd){
    int max = vd.v_end - vd.v_start + 2;
    if( max > wc.rown - 2  )
        max = wc.rown - 2;
    if( wc.cur_pos == max  )
    {
        if (vd.v_end < vfd.size() - 1)
        {
            vd.v_start++;
            vd.v_end++;
        }
        else
            return;
    }
    else
        wc.cur_pos++;
}

//enter key function
void enter(vector <struct file_details> vfd){
    while(!s_forward.empty())
        s_forward.pop();
    int pre = wc.cur_pos - 2 + vd.v_start;
    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    string pres;
    pres.assign(cwd,strlen(cwd));
    if ( vfd[pre].isdir == 1 )
    {
        //if its a directory
        s_backward.push(pres);
        chdir( vfd[pre].path.c_str() );
        vd.v_start = 0;
        wc.cur_pos = 2;
    }
    else{
        //if its a file
        pid_t pid;
       int status;
        if( (pid = vfork()) == -1)
            return;
        else if(pid == 0){
            execlp("vi", "vi", vfd[pre].path.c_str(), (char*)NULL);
        }
        else{
            wait(&status);
        }
    }
}

// function for backspace
void backspace(){
    while(!s_forward.empty())
        s_forward.pop();
    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    string dest;
    dest.assign(cwd,strlen(cwd));
    s_backward.push(dest);
    dest += "/..";
    chdir( dest.c_str() );
    vd.v_start = 0;
    wc.cur_pos = 2;
}

//left arrow key function
void go_back(){
    if(!s_backward.empty())
    {
        char cwd[200];
        getcwd(cwd, sizeof(cwd));
        string dest;
        dest.assign(cwd,strlen(cwd));
        s_forward.push(dest);
        chdir( s_backward.top().c_str());
        vd.v_start = 0;
        s_backward.pop();
        wc.cur_pos = 2;
    }
}

//right arrow key function
void go_forward(){
    if(!s_forward.empty())
    {
        char cwd[200];
        getcwd(cwd, sizeof(cwd));
        string dest;
        dest.assign(cwd,strlen(cwd));
        s_backward.push(dest);
        chdir( s_forward.top().c_str());
        vd.v_start = 0;
        s_forward.pop();
        wc.cur_pos = 2;
    }
}

//h key function
void go_home(){
    while(!s_forward.empty())
        s_forward.pop();
    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    string dest;
    dest.assign(cwd,strlen(cwd));
    if(dest != wc.start_directory){
        s_backward.push(dest);
        chdir( wc.start_directory.c_str());
        vd.v_start = 0;
        wc.cur_pos = 2;
    }
}

//stores command until enter
string command_buffer;

//copies one file to another
void copy_file(char *ad1 , char *ad2 ){
    creat ( ad2 , S_IRWXU | S_IROTH | S_IRGRP );
    int fd1 = open(ad1, O_RDONLY);
    int fd2 = open( ad2, O_RDWR);
    char buffer[4096];
    int s;
    while ( (s = read( fd1, buffer, 4096)) > 0)
        write( fd2, buffer, s );
    close(fd1);
    close(fd2);
    struct stat st;
    stat( ad1, &st);
    chmod(ad2, st.st_mode);
}

// copies recursively
void copy_traverse(char *temp_path , char* des_path)
{
	//struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	mkdir ( des_path , S_IRWXU | S_IROTH | S_IRGRP | S_IXOTH | S_IXGRP );
	dp = opendir( temp_path );
	while ( ( dirp = readdir ( dp ) ) != NULL )
	{	
		struct stat statbuf;
		int l = strlen( temp_path );
        int ld =strlen ( des_path );
        strcat ( temp_path , "/" );
        strcat ( temp_path , dirp -> d_name );
        strcat ( des_path , "/" );
        strcat ( des_path , dirp -> d_name );

		stat( temp_path , &statbuf );
		if ( S_ISDIR( statbuf.st_mode) == 0 ){
              //  cout<< temp_path << " "<< des_path<<endl;
                copy_file(temp_path,des_path);
        }
		else if ( strcmp ( dirp -> d_name , ".." ) != 0 && strcmp ( dirp -> d_name , "." ) != 0 ) 
		{
		//	cout<< temp_path << " "<< des_path<<endl;
			copy_traverse ( temp_path , des_path);
            struct stat st;
            stat( temp_path, &st);
            chmod(des_path, st.st_mode);
		}
		temp_path[ l ] = 0;
        des_path [ ld ] = 0;
	}
	closedir( dp );
	
		
}
void copy_command( vector <string> command_input){
//command_buffer = "copy";
    char temp_path[200]; 
    char des_path[200]; 
    char nr[100];
    char cwd[200];
    char des[200];
    getcwd(cwd, sizeof(cwd));
   // cout << cwd << endl;
    if( command_input [ command_input.size() - 1 ][ 0 ] == '/')
        strcpy(des , command_input[command_input.size() - 1].c_str());
    else{
        strcpy( des , cwd);
        strcat (des , "/");
        strcat(des , command_input[command_input.size() - 1].c_str());
    //    cout << des;
    }
    for ( int i = 1; i < command_input.size() - 1; i++ )
	{
		int l = 0;
		char ar[100];
		if(command_input[i][0]=='/'){
            for ( int j=0; command_input[i][j]!=0; j++)
            {
                if (command_input[i][j]=='/')
                    l = 0;
                else
                    ar[l++] = command_input[i][j];
            }
            ar[l] = 0;
          //  cout<< temp_path << " : " <<des_path <<endl;
            strcpy(temp_path, command_input[i].c_str());
            //cout<< temp_path << " : " <<des_path <<endl;
            strcpy(des_path , des);
            //cout<< temp_path << " : " <<des_path <<endl;
            strcat(des_path, "/");
            //cout<< temp_path << " : " <<des_path <<endl;
            strcat(des_path , ar);
            //cout<< temp_path << " : " <<des_path <<endl;
        }
        else
        {

            strcpy(temp_path, command_input[i].c_str());
            strcpy(des_path , des);
            strcat(des_path, "/");
            strcat(des_path , command_input[i].c_str());
        }
        
        //strcpy(temp_path, argv[i]);
        //strcpy(des_path , ar);
       // cout << "path : " << temp_path << "  :  " << des_path << endl;
        struct stat statbuf;
        stat( temp_path , &statbuf );
        if ( S_ISDIR( statbuf.st_mode) == 0 ){
              //  cout<< temp_path << " "<< des_path<<endl;
                copy_file(temp_path,des_path);
        }
        else
            copy_traverse (temp_path , des_path);
    }
}

//deletes directory recursively
void delete_dir_command( string temp_path){
    struct dirent *dirp;
	DIR *dp;
	dp = opendir( temp_path.c_str() );
	while ( ( dirp = readdir ( dp ) ) != NULL )
	{	
		struct stat statbuf;
		
        string new_path = temp_path + "/";
        string a;
        a.assign(dirp->d_name, strlen(dirp->d_name));
        new_path += a;


		stat( new_path.c_str() , &statbuf );
		if ( S_ISDIR( statbuf.st_mode) == 0 )
                	remove( new_path.c_str() );
		else if ( strcmp ( dirp -> d_name , ".." ) != 0 && strcmp ( dirp -> d_name , "." ) != 0 ) 
		{
			delete_dir_command (new_path );
			rmdir( new_path.c_str() );
		}
		//temp_path[ l ] = 0;
	}
	closedir( dp );
}

//moves files or directories
void move_command( vector <string> command_input){
    copy_command(command_input);
    for( int i = 1; i < command_input.size() - 1; i++){
        struct stat statbuf;
        stat( command_input[i].c_str() , &statbuf );
        if ( S_ISDIR( statbuf.st_mode) == 0 ){
            remove ( command_input[1].c_str() );
        }
        else{
            delete_dir_command(command_input[i]);
            rmdir(command_input[i].c_str());
        }
    }
//command_buffer = "movyy";
}

//rename command
void rename_command( vector <string> command_input){
    rename ( command_input[1].c_str() , command_input[2].c_str() );
}

//create file
void create_file_command( vector <string> command_input){
    char *ar;
	int l = 0;
	if(command_input.size() == 3)
	{
		int k = command_input[2].length(); 
		if ( command_input[2][k-1] == '/')
		{
			l =  command_input[1].length()  + k + 1;
	                ar = new char(l);
        	        strcpy( ar, command_input[2].c_str() );
                	strcat( ar, command_input[1].c_str() );

		}
		else
		{
			l =  command_input[1].length()  + k + 2;
	                ar = new char(l);
        	        strcpy( ar, command_input[2].c_str() );
			strcat( ar, "/");
                        strcat( ar, command_input[1].c_str() );
		}
	}
	else if( command_input.size() == 2)
	{
		l =  command_input[1].length()  + 1;
		ar = new char(l);
		strcpy( ar, command_input[1].c_str() );
	}
	creat ( ar , S_IRWXU | S_IROTH | S_IRGRP );
}

void create_dir_command( vector <string> command_input){
    char *ar;
	int l = 0;
	if(command_input.size() == 3)
	{
		int k =  command_input[2].length();
		if ( command_input[2][k-1] == '/')
		{
			l =  command_input[1].length()  + k + 1;
	                ar = new char(l);
        	        strcpy( ar, command_input[2].c_str() );
                	strcat( ar, command_input[1].c_str() );

		}
		else
		{
			l =  command_input[1].length()  + k + 2;
	                ar = new char(l);
        	        strcpy( ar, command_input[2].c_str() );
			strcat( ar, "/");
                        strcat( ar, command_input[1].c_str() );
		}
	}
	else if( command_input.size() == 2)
	{
		l =  command_input[1].length()  + 1;
		ar = new char(l);
		strcpy( ar, command_input[1].c_str() );
	}
	mkdir ( ar , S_IRWXU | S_IROTH | S_IRGRP | S_IXOTH | S_IXGRP );
}
void delete_file_command( vector <string> command_input){
     remove ( command_input[1].c_str() );
}

void goto_command( vector <string> command_input){
    chdir( command_input[1].c_str() );
}
bool search_command( string cur_path, string target){
	//struct stat statbuf;
	struct dirent *dirp;
	DIR *dp;
	
	dp = opendir( cur_path.c_str() );
	while ( ( dirp = readdir ( dp ) ) != NULL )
	{	
        string a;
        if ( strcmp(dirp -> d_name , target.c_str()) == 0)
            return true; 
		struct stat statbuf;
		//int l = strlen( temp_path );
        
        a.assign(dirp->d_name , strlen(dirp->d_name));
        string new_path = cur_path + "/" + a; 
          //      strcat ( temp_path , "/" );
            //    strcat ( temp_path , dirp -> d_name );

		stat( new_path.c_str() , &statbuf );
		if ( S_ISDIR( statbuf.st_mode) == 0 );
                	//cout << temp_path << endl;
		else if ( strcmp ( dirp -> d_name , "..") != 0 && strcmp ( dirp -> d_name , "." ) != 0 ) 
		{
			//cout << temp_path << endl;
			bool ans = search_command (new_path, target);
            if (ans)
                return true;
		}
		//temp_path[ l ] = 0;
	}
	closedir( dp );
    return false;
}

// takes the input buffer from command
void process_string(string buffer){
    vector <string> command_input;
    string buf = "";
    for( int i=0; i < buffer.length(); i++ )
    {
        if( buffer[i] == ' ')
        {
            command_input.push_back(buf);
            buf.clear();
        }
        else{
            buf.push_back(buffer[i]);
        }
    }
    command_input.push_back(buf);

    if( command_input[0] == "copy"){
        copy_command(command_input);
    }
    else if( command_input[0] == "move" ){
        move_command(command_input);
    }
    else if( command_input[0] == "rename" ){
        rename_command(command_input);
    }
    else if( command_input[0] == "create_file" ){
        create_file_command(command_input);
    }
    else if( command_input[0] == "create_dir" ){
        create_dir_command(command_input);
    }
    else if( command_input[0] == "delete_file" ){
        delete_file_command(command_input);
    }
    else if( command_input[0] == "delete_dir" ){
        delete_dir_command(command_input[1]);
        rmdir(command_input[1].c_str());
    }
    else if( command_input[0] == "goto" ){
        goto_command(command_input);
    }
    else if( command_input[0] == "search" ){
        char cwd[200];
        getcwd(cwd, sizeof(cwd));
        string a;
        a.assign(cwd, strlen(cwd));
        if(search_command(a ,command_input[1]))
            command_buffer = "True";
        else
            command_buffer  = "False";
    }
}
string input_command_buffer;

//switch according to input for character mode
int detect_commands(){
    char c;
    //string buffer = "";    
    
        if( read(STDIN_FILENO, &c, 1)  == 1){
            if (iscntrl(c) && c == 27) {
                read(STDIN_FILENO, &c, 1);
                if( c==91 )
                    read(STDIN_FILENO, &c, 1);
                else
                    return 2;
            }
            else if(iscntrl(c)){
                switch(c){
                    case 13:
                    command_buffer.clear();
                    process_string(input_command_buffer);
                    //cout << buffer << endl;
                    input_command_buffer.clear();
                    return 1;
                    break;
                    case 127:
                    if(!input_command_buffer.empty())
                        input_command_buffer.pop_back();
                   break;
                }
            }
            else{
                input_command_buffer.push_back(c);
                if(input_command_buffer == "q")
                    return 0;
            }
        }
    
    return 1;
    
}
//starts command mode
int command_mode(){
    // clear screen
    // set cursor on bottom
    set_echo_on();
    while( true ){
        write(STDOUT_FILENO, "\x1b[2J", 4);
        //write(STDOUT_FILENO, "\x1b[H", 3);
        update_window_size();
        string ppos = to_string(wc.rown-1*vd.factor );
        string curs = "\x1b["+ ppos + "H";
        const char *ccur = curs.c_str();
        write(STDOUT_FILENO, ccur, ppos.length() + 3);

        cout <<" command mode >> \n";

        ppos = to_string(wc.rown-4*vd.factor );
        curs = "\x1b["+ ppos + "H";
        const char *ccur2 = curs.c_str();
        write(STDOUT_FILENO, ccur2, ppos.length() + 3);
        cout << "Availaible Commands : goto, search, create_file, create_dir\ncopy, move, delete_file, delete_dir, rename\n";

        ppos = to_string(wc.rown-5*vd.factor );
        curs = "\x1b["+ ppos + "H";
        const char *ccur3 = curs.c_str();
        write(STDOUT_FILENO, ccur3, ppos.length() + 3);
        cout << command_buffer << endl;

        ppos = to_string(wc.rown-6*vd.factor );
        curs = "\x1b["+ ppos + "H";
        const char *ccur1 = curs.c_str();
        write(STDOUT_FILENO, ccur1, ppos.length() + 3);

        cout<< input_command_buffer << endl;
        string kh = to_string(input_command_buffer.length() + 1);
        curs = "\x1b["+ ppos + ";" + kh  +"H";
        const char *ccur5 = curs.c_str();
        write(STDOUT_FILENO, ccur5, ppos.length() + 3 + kh.length() + 1);
        int ddc = detect_commands();
        if( ddc == 0)
            return 0;
        else if (ddc == 2)
            break;
    }
    set_echo_of();
    return 1;
}

//switches keys in normal mode
int detect_keys(vector <struct file_details> vfd){
    char c;
    
        if( read(STDIN_FILENO, &c, 1)  == 1){

            if (iscntrl(c) && c == 27) {
                
                read(STDIN_FILENO, &c, 1);

                if( c==91 ){
                    read(STDIN_FILENO, &c, 1);
                    switch(c){
                        case 'A':
                        up(vfd);
                      //  cout << "Up" << endl;
                        break;
                        case 'B':
                        down(vfd);
                      //  cout << "Down" << endl;
                        break;
                        case 'C':
                      //  cout << "Right" << endl;
                        go_forward();
                        break;
                        case 'D':
                        go_back();
                      //  cout << "Left" << endl;
                        break;
                    }
                }
               // else
                 //   cout << "esc" << endl;
            }
            else if(iscntrl(c)){
                switch(c){
                    case 13:
                    enter(vfd);
                   // cout << "enter" << endl;
                    break;
                    case 127:
                    backspace();
                   // cout << "backspace" << endl;
                   break;
                }
            }
                
            
    
            else if (c!='q'){
                switch(c){
                    case 'k':
                    scroll_up(vfd);
                   // cout << "scroll up" << endl;
                    break;
                    case 'l':
                    scroll_down(vfd);
                   // cout << "scroll down" << endl;
                    break;
                    case 'h':
                    go_home();
                  //  cout << "home" << endl;
                    break;
                    case ':':
                    if (command_mode() == 0)
                        return 0;
                   // cout << "colon" << endl;
                   break;
                }
            }
            else
                return 0;
        }
    return 1;
    
}

//executes normal mode
void normal_mode(){
    enter_noncanon();
    char cwd[200];
    getcwd(cwd, sizeof(cwd));
    wc.start_directory.assign(cwd,strlen(cwd));
    
    wc.cur_pos = 2;
    vd.v_start = 0;
    while( true )
    {
        
        write(STDOUT_FILENO, "\x1b[2J", 4);
        write(STDOUT_FILENO, "\x1b[H", 3);
        update_window_size();
        
        vector <struct file_details> vfd=get_file_details();
        if( vd.v_start == 0){
            if( ((wc.rown - (3*vd.factor) ) / vd.factor) > vfd.size() )
                vd.v_end = vfd.size() - 1;
            else
                vd.v_end = (wc.rown - (3*vd.factor) ) / vd.factor;
        }
        string buffer = print_st(vfd );

        cout << buffer;
        int act = vd.factor - 1;

        string ppos = to_string(wc.rown-1*vd.factor);
        string curs = "\x1b["+ ppos + "H";
        const char *ccur = curs.c_str();
        write(STDOUT_FILENO, ccur, ppos.length() + 3);
        cout<< "normal mode >> \n";


        ppos = to_string(wc.cur_pos*vd.factor - act);
        curs = "\x1b["+ ppos + "H";
        const char *ccur1 = curs.c_str();
        write(STDOUT_FILENO, ccur1, ppos.length() + 3);
        
        if( detect_keys(vfd) == 0)
            break;
    }
    
    write(STDOUT_FILENO, "\x1b[2J", 4);
    write(STDOUT_FILENO, "\x1b[H", 3);
    exit_noncanon();
}

int main(){
    normal_mode();    
    return 0;
}

