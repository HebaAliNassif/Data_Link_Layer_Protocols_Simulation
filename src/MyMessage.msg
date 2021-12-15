//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
//
cplusplus {{
#include <bitset>
#include <bits/stdc++.h>
using namespace std;
typedef  std::bitset<8> bits;
}}
class noncobject bits;

packet MyMessage {
    @customize(true);  // see the generated C++ header for more info
    int Message_ID;
    double Sending_Time;
    bits Trailer; //CRC checksum
    int Piggybacking;
    int Piggybacking_ID;
    string Message_Payload;
}
