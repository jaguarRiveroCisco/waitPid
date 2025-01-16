#ifndef PROCESS_INTERFACE_H
#define PROCESS_INTERFACE_H
namespace process
{
    class IProcess 
    {
    public:
        virtual ~IProcess() = default;
        virtual void work() = 0;
    protected:
        virtual void preWork() = 0;
        virtual void postWork() = 0;
    };

} // namespace process

#endif // PROCESS_INTERFACE_H
