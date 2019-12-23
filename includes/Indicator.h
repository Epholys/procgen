#ifndef INDICATOR_H
#define INDICATOR_H


// Very thin base class to add a little indicator to the child class notifying
// that it is modified.
//
// The child object calls 'indicate_modification()' to allow another object to
// call 'poll_modification()' to check if it is modified. Be careful that
// polling the modification resets it.
// Its use is mainly for the composite class to check if its component was
// modified by the interface or another element, to then update itself.
//
// History:
//   This class replace the old 'Observable'/'Observer<>' pattern classes. It
//   was too powerful and too cumbersome with a lot of callback setting
//   everywhere and tiring rule-of-five constructors. It is much more rigid now,
//   not allowing any one-subject/several-observers links, but it was never
//   used, so nothing is lost.
class Indicator
{
public:
    // Defaulted everything
    Indicator() = default;
    virtual ~Indicator() = default;
    Indicator(const Indicator& other) = default;
    Indicator(Indicator&& other) = default;
    Indicator& operator=(const Indicator& other) = default;
    Indicator& operator=(Indicator&& other) = default;

    // Return 'is_modified_' and set it to false.
    // Set to virtual to ease the special case of chained Indicators.
    virtual bool poll_modification();

protected:
    // Set 'is_modified_' to true.
    void indicate_modification();

private:
    // Flag to indicate another class if the object is meaningfully modified.
    bool is_modified_ {false};
};

#endif // INDICATOR_H
