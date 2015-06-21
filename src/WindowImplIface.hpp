#ifndef R64FX_WINDOW_IMPL_IFACE_HPP
#define R64FX_WINDOW_IMPL_IFACE_HPP

namespace r64fx{

/** @brief Window interface to be given to GUI implementation. */
class WindowImplIface{
    void* m_impl_data = nullptr;

public:
    inline void setImplData(void* data) { m_impl_data = data; }

    inline void* getImplData() const { return m_impl_data; }
};

}//namespace r64fx

#endif//R64FX_WINDOW_IMPL_IFACE_HPP