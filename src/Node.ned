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

package networksproject_v2;

//
// TODO auto-generated module
//
simple Node
{
    double delay_amount;
    double prop_delay;
    double timeout;
    int window_size;
    double timeout_ack;
    double interval_between_msgs;
    volatile int index_of_error_bit = intuniform(0, 100000);
    bool hamming;
     gates:
        input in_pair;
        input in_coordinator;
        output out_coordinator;
        output out_pair;
}
