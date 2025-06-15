# 🚗 License Plate Management System

This project implements a License Plate Management System in C++ using a **custom Red-Black Tree** data structure. It efficiently supports plate registration, deletion, lexicographic lookup, and revenue tracking for custom and randomly generated license plates.

## 🔧 Features

- Register new license plates (custom or randomly generated)
- Delete existing license plates
- Lookup plate existence, previous and next plates
- Query plates within a lexicographical range
- Track total revenue based on plate type (custom: 7 Galleons, random: 4 Galleons)
- Read/write commands and output via files

## ⚙️ Technical Details

- **Data Structure**: Fully implemented Red-Black Tree (no STL used)
- **Performance**: All key operations in `O(log n)` time
- **CLI-Based**: Commands are read from a file and output is generated accordingly

## 🏁 Getting Started

### Compile:
```bash
g++ -std=c++11 main.cpp -o LicensePlateManager
```

### Run:
```bash
./LicensePlateManager input.txt
```

## 📂 Input Commands Supported
- `addLicence [plate]`
- `dropLicence [plate]`
- `lookupLicence [plate]`
- `lookupPrev [plate]`
- `lookupNext [plate]`
- `lookupRange [low] [high]`
- `revenue`

## 📈 Sample Output
All outputs are written to `[input_filename]_output.txt`.

## 👨‍💻 Author
Trinesh Reddy Bayapureddy Sannala  
UFID: 6264-8646  
Email: bayapureddysa.tr@ufl.edu
