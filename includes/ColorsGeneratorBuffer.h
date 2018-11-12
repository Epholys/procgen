#ifndef COLOR_GENERATOR_BUFFER_H
#define COLOR_GENERATOR_BUFFER_H

#include "ColorsGenerator.h"

namespace colors
{
    class ColorGeneratorBuffer : public Observable
    {
    public:
        ColorGeneratorBuffer();
        ColorGeneratorBuffer(std::shared_ptr<ColorGenerator> gen);

        std::shared_ptr<ColorGenerator> get_generator() const;
        void set_generator(std::shared_ptr<ColorGenerator> gen);

    private:
        std::shared_ptr<ColorGenerator> generator_;
    };
}


#endif // COLOR_GENERATOR_BUFFER_H
