using namespace System::Threading;

ref class Lock
{
    Object^ m_object;
public:
    Lock(Object^ object) : m_object(object)
    {
        Monitor::Enter(m_object);
    }

    ~Lock()
    {
        Monitor::Exit(m_object);
    }
};
