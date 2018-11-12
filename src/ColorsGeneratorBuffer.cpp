#include "ColorsGeneratorBuffer.h"

namespace colors
{
    ColorGeneratorBuffer::ColorGeneratorBuffer()
        : generator_{std::make_shared<ConstantColor>()}
    {
    }
    
                                      
    ColorGeneratorBuffer::ColorGeneratorBuffer(std::shared_ptr<ColorGenerator> gen)
        : generator_{gen}
    {
    }

    std::shared_ptr<ColorGenerator> ColorGeneratorBuffer::get_generator() const
    {
        return generator_;
    }

    void ColorGeneratorBuffer::set_generator(std::shared_ptr<ColorGenerator> gen)
    {
        generator_ = gen;
        notify();
    }
}
