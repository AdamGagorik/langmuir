#include "rand.h"
#include <fstream>
#include <sstream>

namespace LangmuirCore
{

Random::Random(quint64 seed, QObject *parent) : QObject(parent)
{
    m_seed = 0;
    if(seed == 0)
    {
        m_seed = static_cast < unsigned int >(time(0));
    }
    else
    {
        m_seed = static_cast < unsigned int >(seed);
    }
    twister = new boost::mt19937(m_seed);

    boost::uniform_01< double > distribution;
    generator01 = new boost::variate_generator < boost::mt19937 &, boost::uniform_01 < double > >(*twister, distribution);
}

Random::~Random()
{
    delete twister;
    delete generator01;
}

quint64 Random::seed()
{
    return m_seed;
}

void Random::seed(quint64 seed)
{
    if(seed == 0)
    {
        m_seed = static_cast < unsigned int >(time(0));
    }
    else
    {
        m_seed = static_cast < unsigned int >(seed);
    }
    twister->seed(m_seed);
}

double Random::random()
{
    return(*generator01)();
}

double Random::range(const double low, const double high)
{
    boost::uniform_real < double > distribution(low, high);
    boost::variate_generator < boost::mt19937 &, boost::uniform_real < double > >generator(*twister, distribution);
    return generator();
}

double Random::normal(const double mean, const double sigma)
{
    boost::normal_distribution<double> distribution(mean, sigma);
    boost::variate_generator < boost::mt19937 &, boost::normal_distribution < double > >  generator(*twister, distribution);
    return generator();
}

int Random::integer(const int low, const int high)
{
    boost::uniform_int < int > distribution(low, high);
    boost::variate_generator < boost::mt19937 &, boost::uniform_int < int > >generator(*twister, distribution);
    return generator();
}

bool Random::metropolis(double energyChange, double inversekT)
{
    if(energyChange > 0.0)
    {
        if(exp(-energyChange * inversekT) > this->random())
        {
            return true;
        }
    }
    else
    {
        return true;
    }
    return false;
}

bool Random::metropolisWithCoupling(double energyChange, double inversekT, double coupling)
{
    double randNumber = this->random();
    if(energyChange > 0.0)
    {
        if(coupling * exp(-energyChange * inversekT)> randNumber)
        {
            return true;
        }
    }
    else if(coupling > randNumber)
    {
        return true;
    }
    return false;
}

bool Random::chooseNo(double percent)
{
    if(percent < this->random())
    {
        return true;
    }
    return false;
}

bool Random::chooseYes(double percent)
{
    if(percent > this->random())
    {
        return true;
    }
    return false;
}

QDataStream& operator<<(QDataStream& stream, Random& random)
{
    std::stringstream sstream;
    sstream << *random.twister;
    if (sstream.fail() || sstream.bad())
    {
        qFatal("langmuir: can not save state of random number generator to QDataStream; "
               "std::stringstream has failed on write");
    }
    QList<quint64> state;
    quint64 value;
    sstream >> value;
    while(!sstream.eof())
    {
        if (sstream.fail() || sstream.bad())
        {
            qFatal("langmuir: can not save state of random number generator to QDataStream; "
                   "std::stringstream has failed on read");
        }
        state.push_back(value);
        sstream >> value;
    }
    stream << quint64(random.m_seed);
    stream << state;
    return stream;
}

QDataStream& operator>>(QDataStream& stream, Random& random)
{
    stream >> random.m_seed;
    switch (stream.status())
    {
    case QDataStream::Ok:
    {
        break;
    }
    case QDataStream::ReadPastEnd:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream, random.seed: "
               "QDataStream::ReadPastEnd");
        break;
    }
    case QDataStream::ReadCorruptData:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream, random.seed: "
               "QDataStream::ReadCorruptData");
        break;
    }
    default:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream, random.seed: "
               "QDataStream::Status unknown");
    break;
    }
    }
    QList<quint64> state;
    stream >> state;
    switch (stream.status())
    {
    case QDataStream::Ok:
    {
        break;
    }
    case QDataStream::ReadPastEnd:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream: "
               "QDataStream::ReadPastEnd");
        break;
    }
    case QDataStream::ReadCorruptData:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream: "
               "QDataStream::ReadCorruptData");
        break;
    }
    default:
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream: "
               "QDataStream::Status unknown");
    break;
    }
    }
    std::stringstream sstream;
    for (int i = 0; i < state.size(); i++)
    {
        sstream << state.at(i) << " ";
        if (sstream.fail() || sstream.bad())
        {
            qFatal("langmuir: can not load state of random number generator from QDataStream; "
                   "std::stringstream has failed on write");
        }
    }
    sstream >> *random.twister;
    if (sstream.fail() || sstream.bad())
    {
        qFatal("langmuir: can not load state of random number generator from QDataStream; "
               "std::stringstream has failed on read");
    }
    return stream;
}

QTextStream& operator<<(QTextStream& stream, Random& random)
{
    std::stringstream sstream;
    sstream << *random.twister;
    if (sstream.fail() || sstream.bad())
    {
        qFatal("langmuir: can not save state of random number generator to QTextStream; "
               "std::stringstream has failed on write");
    }
    QList<quint64> state;
    quint64 value;
    sstream >> value;
    while(!sstream.eof())
    {
        if (sstream.fail() || sstream.bad())
        {
            qFatal("langmuir: can not save state of random number generator to QTextStream; "
                   "std::stringstream has failed on read");
        }
        state.push_back(value);
        sstream >> value;
    }
    stream << quint64(random.m_seed);
    stream << " " << quint64(state.size());
    for (int i = 0; i < state.size(); i++)
    {
        stream << " " << quint64(state.at(i));
    }
    return stream;
}

QTextStream& operator>>(QTextStream& stream, Random& random)
{
    stream >> random.m_seed;
    switch (stream.status())
    {
    case QTextStream::Ok:
    {
        break;
    }
    case QTextStream::ReadPastEnd:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, random.seed: "
               "QTextStream::ReadPastEnd");
        break;
    }
    case QTextStream::ReadCorruptData:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, random.seed: "
               "QTextStream::ReadCorruptData");
        break;
    }
    default:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, random.seed: "
               "QTextStream::Status unknown");
    break;
    }
    }
    quint64 size;
    stream >> size;
    switch (stream.status())
    {
    case QTextStream::Ok:
    {
        break;
    }
    case QTextStream::ReadPastEnd:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, state.size(): "
               "QTextStream::ReadPastEnd");
        break;
    }
    case QTextStream::ReadCorruptData:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, state.size(): "
               "QTextStream::ReadCorruptData");
        break;
    }
    default:
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream, state.size(): "
               "QTextStream::Status unknown");
    break;
    }
    }
    std::stringstream sstream;
    for (int i = 0; i < int(size); i++)
    {
        quint64 value = 0;
        stream >> value;
        switch (stream.status())
        {
        case QTextStream::Ok:
        {
            break;
        }
        case QTextStream::ReadPastEnd:
        {
            qFatal("langmuir: can not load state of random number generator from QTextStream: state.at(%d)"
                   "QTextStream::ReadPastEnd",i);
            break;
        }
        case QTextStream::ReadCorruptData:
        {
            qFatal("langmuir: can not load state of random number generator from QTextStream: state.at(%d)"
                   "QTextStream::ReadCorruptData",i);
            break;
        }
        default:
        {
            qFatal("langmuir: can not load state of random number generator from QTextStream: state.at(%d)"
                   "QTextStream::Status unknown",i);
        break;
        }
        }
        sstream << value << " ";
        if (sstream.fail() || sstream.bad())
        {
            qFatal("langmuir: can not load state of random number generator from QTextStream; state.at(%d)"
                   "std::stringstream has failed on write", i);
        }
    }
    sstream >> *random.twister;
    if (sstream.fail() || sstream.bad())
    {
        qFatal("langmuir: can not load state of random number generator from QTextStream; "
               "std::stringstream has failed on read");
    }
    return stream;
}

std::ostream& operator<<(std::ostream& stream, Random& random)
{
    stream << random.m_seed << ' ';
    stream << *random.twister;
    return stream;
}

std::istream& operator>>(std::istream& stream, Random& random)
{
    stream >> random.m_seed;
    if (stream.fail() || stream.bad() || stream.eof())
    {
        qFatal("langmuir: can not load state of random number generator; random.m_seed\n"
               "std::ifstream has failed on write");
    }
    stream >> *random.twister;
    if (stream.fail() || stream.bad() || stream.eof())
    {
        qFatal("langmuir: can not load state of random number generator; twister\n"
               "std::ifstream has failed on write");
    }
    return stream;
}

}
