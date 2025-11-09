#pragma once

namespace edvar::app {
class application_base : public memory::enable_shared_from_this<application_base>{
protected:
    application_base(
        string application_name = L"edvar cpp core application",
        bool should_be_dpi_aware = true,
        bool register_automatically = true
    );
    virtual ~application_base();

    virtual void on_startup() {}
    virtual void on_tick(double delta_time) {}
    virtual void on_suspend() {}
    virtual void on_resume() {}
    virtual void on_shutdown() {}

    void poll_events();

    virtual void on_os_locale_changed(const edvar::internationalization::locale& new_locale,
                                      const edvar::internationalization::locale& old_locale) {}
    virtual void on_os_input_language_changed(const edvar::internationalization::locale& new_language,
                                              const edvar::internationalization::locale& old_language) {}
    enum class os_theme : uint8_t { unknown, light, dark };
    virtual void on_os_theme_changed(const os_theme& new_theme, const os_theme& old_theme) {}
    enum class os_system_power_state : uint8_t {
        unknown,
        on_battery,
        charging,
        hibernating,
    };
    virtual void on_os_power_state_changed(const os_system_power_state& new_state,
                                           const os_system_power_state& old_state) {}
    virtual void on_os_clipboard_changed(const array<uint8_t>& new_data) {}

private:
    string _application_name;
};
} // namespace edvar::app