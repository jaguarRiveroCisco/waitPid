#ifndef FILEDESCRIPTOR_H
#define FILEDESCRIPTOR_H

namespace filesystem
{
    class FileDescriptor
    {
    public:
        explicit FileDescriptor(const std::string &fileName);
        ~FileDescriptor();

        [[nodiscard]] int get() const;

    private:
        int fd_;
    };

}

#endif // FILEDESCRIPTOR_H
