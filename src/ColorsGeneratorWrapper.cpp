#include "ColorsGeneratorWrapper.h"

namespace colors
{
    ColorGeneratorWrapper::ColorGeneratorWrapper()
        : Observable{}
        , OGen{std::make_shared<ConstantColor>()}
    {
        add_callback([this](){notify();});
    }
                                         
    ColorGeneratorWrapper::ColorGeneratorWrapper(std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , OGen{gen}
    {
        add_callback([this](){notify();});
    }

    ColorGeneratorWrapper::ColorGeneratorWrapper(const ColorGeneratorWrapper& other)
        : Observable{}
        , OGen{other.get_target()->clone()}
    {
        add_callback([this](){notify();});
    }
    ColorGeneratorWrapper::ColorGeneratorWrapper(ColorGeneratorWrapper&& other)
        : Observable{}
        , OGen{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        other.set_target(nullptr);
    }
    ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(const ColorGeneratorWrapper& other)
    {
        if (this != &other)
        {
            set_target(other.get_target()->clone());
            add_callback([this](){notify();});
        }
        return *this;
    }
    ColorGeneratorWrapper& ColorGeneratorWrapper::operator=(ColorGeneratorWrapper&& other)
    {
        if (this != &other)
        {
            set_target(std::move(other.get_target()));
            add_callback([this](){notify();});
            other.set_target(nullptr);
        }
        return *this;
    }

    // ColorGeneratorWrapper ColorGeneratorWrapper::clone() const
    // {
    //     return ColorGeneratorWrapper(get_target()->clone());
    // }
    
    std::shared_ptr<ColorGenerator> ColorGeneratorWrapper::unwrap() const
    {
        return get_target();
    }

    void ColorGeneratorWrapper::wrap(std::shared_ptr<ColorGenerator> gen)
    {
        set_target(gen);
        add_callback([this](){notify();});
        notify();
    }
}
