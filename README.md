# sequence_view

## Motivation

I want a simple, portable and C++20 (or even C++17) compatible way of accesing continuous sequences of data that are representing multi-dimensional arrays.
Goal is to be able to access data in such easy way like a NumPy array allows to.

## Future work
[ ] Simple view for 1D sequence
[ ] Simple view for 2D sequence
[ ] Generalize for N-D sequences
[ ] Access data with fixed step
[ ] Access data with mask
[ ] Modify data with single value assignement
[ ] Modify data view -> view
[ ] Modify data view(mask) -> view(mask)

## Examples (TBD) 

Syntax can change in the future to make it more convenient for end user.

```c++
uint64_t data[100];
std::vector<double> sequence;
sequence.resize(100);
// Full view of data - 2D matrix
SeqView::View view(data, SeqView::Size(10, 10));
SeqView::View other(data, SeqView::Size(10, 10));
SeqView::View other(sequence, SeqView::Size(10, -1)); // 2D but can be deduced
... // Fill with random data
// Partial view - 2D slice of 2 rows x 10 cols 
auto v = view(SeqView::Range(3,5)); // second argument skipped
v = 11; // assign single value
// Range of 1 element can be either X or X:X+1
auto o = other(SeqView::Range(4,6), SeqView::Range(1)); // second range is single column
v = o; // Views size match
auto mask = view > 10; // mask for large elements
other(mask) = view(mask); // copy elements to other matrix
view(mask) = 0; // zero elements
auto max_elements = SeqView::max(other, view); // custom algorithm
auto other_with_max = other; // just copy
other_with_max(other < max) = max; // not sure if that will be confusing or not
auto reshaped = other.reshape(-1, 5); // first argument (only one) can be calculated -> 20
view(SeqView::Range(2,5,2)); // Access 3rd, 5th row, full specification
view(SeqView::StepRange(2)); // Access 1st, 3rd, ... row
view(SeqView::StopRange(3)); // Second argument is 1 by default access 1st, 2nd and 3rd row
view(SeqView::StartRange(2)); // Second argument is 1 by default access 3rd, 4th ... row
// Start, stop and step can be negative, but out of bound range can not be constructed
```