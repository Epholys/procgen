#include "ColorsGeneratorBuffer.h"

namespace colors
{
    ColorGeneratorBuffer::ColorGeneratorBuffer()
        : Observable{}
        , OGen{std::make_shared<ConstantColor>()}
    {
        add_callback([this](){notify();});
    }
                                         
    ColorGeneratorBuffer::ColorGeneratorBuffer(std::shared_ptr<ColorGenerator> gen)
        : Observable{}
        , OGen{gen}
    {
        add_callback([this](){notify();});
    }

    ColorGeneratorBuffer::ColorGeneratorBuffer(const ColorGeneratorBuffer& other)
        : Observable{}
        , OGen{other.get_target()}
    {
        add_callback([this](){notify();});
    }
    ColorGeneratorBuffer::ColorGeneratorBuffer(ColorGeneratorBuffer&& other)
        : Observable{}
        , OGen{std::move(other.get_target())}
    {
        add_callback([this](){notify();});
        other.set_target(nullptr);
    }
    ColorGeneratorBuffer& ColorGeneratorBuffer::operator=(const ColorGeneratorBuffer& other)
    {
        if (this != &other)
        {
            OGen {other.get_target()};
            add_callback([this](){notify();});
        }
        return *this;
    }
    ColorGeneratorBuffer& ColorGeneratorBuffer::operator=(ColorGeneratorBuffer&& other)
    {
        if (this != &other)
        {
            OGen {other.get_target()};
            add_callback([this](){notify();});
            other.set_target(nullptr);
        }
        return *this;
    }

    std::shared_ptr<ColorGenerator> ColorGeneratorBuffer::get_generator() const
    {
        return get_target();
    }

    void ColorGeneratorBuffer::set_generator(std::shared_ptr<ColorGenerator> gen)
    {
        set_target(gen);
        add_callback([this](){notify();});
        notify();
    }
}
