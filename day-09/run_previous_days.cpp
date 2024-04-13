#include <iostream>
#include "day-09.hpp"

using namespace std;

void run_day_2(){
    cout << "Day 2" << endl;
    {
        cout << "Example 1:" << endl;
        ifstream inf{"../day-02/inex-02.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.interpret();
        for (auto& x: c.program) cout << x << ", ";
        cout << endl;
        int expected = 3500;
        int actual = c.program.at(0);
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }

    cout << "Example 2:" << endl;
    {
        ifstream inf{"../day-02/inex-02-2.txt"};
        vector<long> program{my_parse(inf)};
        Computer c2{program};
        c2.interpret();
        for (auto& x: c2.program) cout << x << ", ";
        cout << endl;
        int expected = 2;
        int actual = c2.program.at(0);
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
        }

    cout << "Example 3:" << endl;
    {
        ifstream inf{"../day-02/inex-02-3.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.interpret();
        for (auto& x: c.program) cout << x << ", ";
        cout << endl;
        int expected = 6;
        int actual = c.program.at(3);
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
        }

    cout << "Example 4:" << endl;
    {
        ifstream inf{"../day-02/inex-02-4.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.interpret();
        for (auto& x: c.program) cout << x << ", ";
        cout << endl;
        int expected = 9801;
        int actual = c.program.at(5);
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
        }

    cout << "Example 5:" << endl;
    {
        ifstream inf{"../day-02/inex-02-5.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.interpret();
        for (auto& x: c.program) cout << x << ", ";
        cout << endl;
        int expected = 30;
        int actual = c.program.at(0);
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
        }

    cout << "Me (part 1 only):" << endl;
    {
        ifstream inf{"../day-02/inme-02.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.program.at(1) = 12;
        c.program.at(2) = 2;
        c.interpret();
        cout << "Final: " << c.program.at(0) << endl;
    }
    return;
}

void run_day_5(){
    cout << "Day 5" << endl;
    {
        cout << "Example 1-1:" << endl;
        ifstream inf{"../day-05/inex-05-1.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(8);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 1-2:" << endl;
        ifstream inf{"../day-05/inex-05-1.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(9);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 2-1:" << endl;
        ifstream inf{"../day-05/inex-05-2.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(7);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 2-2:" << endl;
        ifstream inf{"../day-05/inex-05-2.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(8);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 3-1:" << endl;
        ifstream inf{"../day-05/inex-05-3.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(8);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 3-2:" << endl;
        ifstream inf{"../day-05/inex-05-3.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(9);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 4-1:" << endl;
        ifstream inf{"../day-05/inex-05-4.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(7);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 4-2:" << endl;
        ifstream inf{"../day-05/inex-05-4.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(8);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 5-1:" << endl;
        ifstream inf{"../day-05/inex-05-5.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(0);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 5-2:" << endl;
        ifstream inf{"../day-05/inex-05-5.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(1);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 6-1:" << endl;
        ifstream inf{"../day-05/inex-05-6.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(0);
        c.interpret();
        int expected = 0;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 6-2:" << endl;
        ifstream inf{"../day-05/inex-05-6.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(1);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 7-1:" << endl;
        ifstream inf{"../day-05/inex-05-7.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(7);
        c.interpret();
        int expected = 999;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 7-2:" << endl;
        ifstream inf{"../day-05/inex-05-7.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(8);
        c.interpret();
        int expected = 1000;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Example 7-3:" << endl;
        ifstream inf{"../day-05/inex-05-7.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(9);
        c.interpret();
        int expected = 1001;
        int actual = c.output_buffer.front();
        cout << '[' << (expected == actual ? 'X' : ' ' ) << ']' << ' ' << actual <<
        " vs " << expected << " (expected)" << endl;
    }
    {
        cout << "Me, part 2:" << endl;
        ifstream inf{"../day-05/inme-05.txt"};
        vector<long> program{my_parse(inf)};
        Computer c{program};
        c.input_buffer.push_back(5);
        c.interpret();
        int expected = 1;
        int actual = c.output_buffer.front();
        cout << "Final: " << actual << endl;
    }
    
    return;
}