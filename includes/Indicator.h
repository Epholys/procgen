#ifndef INDICATOR_H
#define INDICATOR_H

class Indicator
{
public:
    Indicator() = default;
    virtual ~Indicator() = default;
    Indicator(const Indicator& other) = default;
    Indicator(Indicator&& other) = default;
    Indicator& operator=(const Indicator& other) = default;
    Indicator& operator=(Indicator&& other) = default;

    bool poll_modification();

protected:
    void indicate_modification();

private:
    bool is_modified_ {false};
};

#endif // INDICATOR_H
