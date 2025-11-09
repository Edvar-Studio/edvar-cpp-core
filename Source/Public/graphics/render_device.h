#pragma once


namespace edvar::graphics {
class render_device : public memory::enable_shared_from_this<render_device> {
public:
    static memory::shared_pointer<render_device> create();

protected:
    render_device() = default;
    virtual ~render_device() = default;
};
}