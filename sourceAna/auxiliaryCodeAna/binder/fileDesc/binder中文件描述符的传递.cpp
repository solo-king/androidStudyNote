写入fd
Pacel reply->writeFileDescriptor(fd, )
取出fd
int fd = pracel->readFileDescriptor();
Note:
		对于传递的文件句柄，应该传递其副本。
			因为在Pacel的析构函数中会关闭fd
			--有疑问:
					关闭fd时只是去除引用吗?