#define FUSE_USE_VERSION 28
#include <fuse.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <dirent.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/wait.h>

static  const  char * dirpath = "/home/geizka/Documents";
char key[100] = "9(ku@AW1[Lmvgax6q`5Y2Ry?+sF!^HKQiBXCUSe&0M.b%rI'7d)o4~VfZ*{#:}ETt$3J-zpc]lnh8,GwP_ND|jO";
char encv1[10] = "encv1_";
char encv2[10] = "encv2_";

void encription1WithLength(char* enc, int length) {
	if(strcmp(enc, ".") == 0 || strcmp(enc, "..") == 0)return;
	for(int i = length; i >= 0; i--){
		if(enc[i]=='/')break;
		if(enc[i]=='.'){
			length = i;
			break;
		}
	}
	int start = 0;
	for(int i = 0; i < length; i++){
		if(enc[i] == '/')start = i+1;
	}
    for ( int i = start; i < length; i++) {
		if(enc[i]=='/')continue;
        for (int j = 0; j < 87; j++) {
            if(enc[i] == key[j]) {
                enc[i] = key[(j+10) % 87];
                break;
            }
        }
    }
}

void decription1WithLength(char * enc, int length){
	if(strcmp(enc, ".") == 0 || strcmp(enc, "..") == 0)return;
	if(strstr(enc, "/") == NULL)return;
	for(int i = length; i >= 0; i--){
		if(enc[i]=='/')break;
		if(enc[i]=='.'){
			length = i;
			break;
		}
	}
	int start = 0;
	for(int i = 0; i < length; i++){
		if(enc[i] == '/'){
			start = i+1;
			break;
		}
	}
    for ( int i = start; i < length; i++) {
		if(enc[i]=='/')continue;
        for (int j = 0; j < 87; j++) {
            if(enc[i] == key[j]) {
                enc[i] = key[(j+77) % 87];
                break;
            }
        }
    }
}

void encription1(char* enc) {
	encription1WithLength(enc, strlen(enc));
}

void decription1(char* enc){
	decription1WithLength(enc, strlen(enc));
}

static  int  xmp_getattr(const char *path, struct stat *stbuf){
	printf("\n\nDEBUG getattr\n\n");
	printf("DEBUGGING %s\n", path);
	char * enc1Ptr = strstr(path, encv1);
	if(enc1Ptr != NULL)
		decription1(enc1Ptr);
	printf("DEBUGGING %s\n", path);
	int res;
	char fpath[1000];
	sprintf(fpath,"%s%s", dirpath, path);
	res = lstat(fpath, stbuf);
	if (res == -1)
		return -errno;
	return 0;
}

static int xmp_readdir(const char *path, void *buf, fuse_fill_dir_t filler, off_t offset, struct fuse_file_info *fi){

	char * enc1Ptr = strstr(path, encv1);
	if(enc1Ptr != NULL)
		decription1(enc1Ptr);
	

	printf("\n\nDEBUG readdir\n\n");

	char fpath[1000];
	if(strcmp(path,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	} else sprintf(fpath, "%s%s",dirpath,path);

	int res = 0;
	DIR *dp;
	struct dirent *de;
	(void) offset;
	(void) fi;
	dp = opendir(fpath);
	if (dp == NULL)
		return -errno;

	while ((de = readdir(dp)) != NULL) {
		struct stat st;
		memset(&st, 0, sizeof(st));
		st.st_ino = de->d_ino;
		st.st_mode = de->d_type << 12;
		if(enc1Ptr != NULL)
			encription1(de->d_name);
		res = (filler(buf, de->d_name, &st, 0));
		if(res!=0) break;
	}
	closedir(dp);

	return 0;
}

static int xmp_mkdir(const char *path, mode_t mode){

	printf("\n\nDEBUG mkdir\n\n");

	char fpath[1000];
	if(strcmp(path,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	} else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = mkdir(fpath, mode);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_mknod(const char *path, mode_t mode, dev_t rdev){

	printf("\n\nDEBUG mknod\n\n");

	char fpath[1000];
	if(strcmp(path,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	} else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	if (S_ISREG(mode)) {
		res = open(fpath, O_CREAT | O_EXCL | O_WRONLY, mode);
		if (res >= 0)
			res = close(res);
	} else if (S_ISFIFO(mode))
		res = mkfifo(fpath, mode);
	else
		res = mknod(fpath, mode, rdev);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_unlink(const char *path) {

	char * enc1Ptr = strstr(path, encv1);
	if(enc1Ptr != NULL)
		decription1(enc1Ptr);

	printf("\n\nDEBUG unlink\n\n");

	char fpath[1000];
	if(strcmp(path,"/") == 0){
		path=dirpath;
		sprintf(fpath,"%s",path);
	} else sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = unlink(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rmdir(const char *path) {

	char * enc1Ptr = strstr(path, encv1);
	if(enc1Ptr != NULL)
		decription1(enc1Ptr);

	printf("\n\nDEBUG rmdir\n\n");

	char fpath[1000];
	sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = rmdir(fpath);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_rename(const char *from, const char *to) {

	printf("\n\nDEBUG rename\n\n");

	char ffrom[1000];
	char fto[1000];
	sprintf(ffrom, "%s%s",dirpath, from);
	sprintf(fto, "%s%s",dirpath, to);
	int res;

	int dirIndex = 0;
	int length = strlen(fto);
	for(int i = length; i >= 0; i--){
		if(fto[i] == '/'){
			dirIndex = i;
			break;
		}
	}

	//check if from contain encv1

	char dir[1000];
	strncpy(dir, fto, dirIndex);
	pid_t id = fork();
	if(id){
		wait(NULL);
	}else{
		char * arg[]={"mkdir", "-p", dir, NULL};
		execv("/bin/mkdir", arg);
	}


	res = rename(ffrom, fto);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_truncate(const char *path, off_t size) {

	printf("\n\nDEBUG truncate\n\n");

	char fpath[1000];
	sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = truncate(fpath, size);
	if (res == -1)
		return -errno;

	return 0;
}

static int xmp_open(const char *path, struct fuse_file_info *fi){

	printf("\n\nDEBUG open\n\n");

	char fpath[1000];
	sprintf(fpath, "%s%s",dirpath,path);
	int res;

	res = open(fpath, fi->flags);
	if (res == -1)
		return -errno;

	close(res);
	return 0;
}

static int xmp_read(const char *path, char *buf, size_t size, off_t offset, struct fuse_file_info *fi){
	
	printf("\n\nDEBUG read\n\n");
	
	char fpath[1000];
	sprintf(fpath, "%s%s",dirpath,path);
	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_RDONLY);
	if (fd == -1)
		return -errno;

	res = pread(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}

static int xmp_write(const char *path, const char *buf, size_t size, off_t offset, struct fuse_file_info *fi) {
	
	printf("\n\nDEBUG write\n\n");
	
	char fpath[1000];
	sprintf(fpath, "%s%s", dirpath, path);
	int fd;
	int res;

	(void) fi;
	fd = open(fpath, O_WRONLY);
	
	if (fd == -1)
		return -errno;

	res = pwrite(fd, buf, size, offset);
	if (res == -1)
		res = -errno;

	close(fd);
	return res;
}


static struct fuse_operations xmp_oper = {
	.getattr = xmp_getattr,
	.readdir = xmp_readdir,
	.read = xmp_read,
	.mkdir = xmp_mkdir,
	.mknod = xmp_mknod,
	.unlink = xmp_unlink,
	.rmdir = xmp_rmdir,
	.rename = xmp_rename,
	.truncate = xmp_truncate,
	.open = xmp_open,
	.read = xmp_read,
	.write = xmp_write,

};

int  main(int  argc, char *argv[]){
	umask(0);
	return fuse_main(argc, argv, &xmp_oper, NULL);
}