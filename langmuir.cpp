#include "inputparser.h"
#include "cubicgrid.h"
#include "simulation.h"

#include <QtCore/QCoreApplication>
#include <QtCore/QStringList>
#include <QtCore/QFile>
#include <QtCore/QTextStream>
#include <QtCore/QDateTime>
#include <QtCore/QDebug>

using namespace std;
using namespace Langmuir;

int main(int argc, char *argv[])
{
  QCoreApplication app(argc, argv);
  QStringList args = app.arguments();

  if (args.size() < 2) {
    qDebug() << "correct use is langmuir input.dat (output.dat)";
    throw(std::invalid_argument("bad input"));
  }

  QString  inputFileName = args.at(1);
  QString outputFileName = args.size() > 2 ? args.at(2) : args.at(1);

  // Now open our input file
  QFile inputFile(inputFileName);
  if (!inputFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
    qDebug() << "Error opening intput file:" << inputFileName;
    app.exit(1);
  }
  inputFile.close();
  InputParser input(inputFileName);
  SimulationParameters par;
  input.simulationParameters(&par);

  // Open our main output file and get it ready for writing
  QFile outputFile(outputFileName+".dat");
  if (!outputFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
    qDebug() << "Error opening output file:" << outputFileName + ".dat";
    app.exit(1);
  }
  QTextStream out(&outputFile);

  QDateTime startDateTime(QDateTime::currentDateTime());
  // Output a summary of all simulation parameters
  out << "Langmuir Simulation Code\nHutchison Group\n\n"
      << "Started: "                << startDateTime.toString("hh:mm:ss d MMMM, yyyy")
      << "\n\nvariable.working: "   << input.workingVariable()
      << "\nvariable.start: "       << input.start()
      << "\nvariable.final: "       << input.final()
      << "\nvariable.steps: "       << input.steps()
      << "\niterations.warmup: "    << par.iterationsWarmup
      << "\niterations.real: "      << par.iterationsReal
      << "\niterations.print: "     << par.iterationsPrint
      << "\niterations.traj: "      << par.iterationsTraj
      << "\ngrid.width: "           << par.gridWidth
      << "\ngrid.height: "          << par.gridHeight
      << "\ngrid.depth: "           << par.gridDepth
      << "\ngrid.charge: "          << par.gridCharge
      << "\ninteractions.coulomb: " << par.coulomb
      << "\ncharged.defects:  "     << par.defectsCharged
      << "\ncharged.traps: "        << par.trapsCharged
      << "\nz.defect:  "            << par.zDefect
      << "\nz.trap:    "            << par.zTrap
      << "\nvoltage.source: "       << par.voltageSource
      << "\nvoltage.drain: "        << par.voltageDrain
      << "\npotential.form: "       << par.potentialForm
      << "\ndefect.percentage: "    << par.defectPercentage * 100.0
      << "\ntrap.percentage: "      << par.trapPercentage * 100.0
      << "\ncharge.percentage: "    << par.chargePercentage * 100.0
      << "\ntemperature.kelvin: "   << par.temperatureKelvin
      << "\ndelta.epsilon: "        << par.deltaEpsilon
      << "\n\n";

  // Now output the column titles
  out << "#i "              << "(n)" << "\t";
  out << "Temperature "     << "(K)" << "\t";
  out << "Source "          << "(V)" << "\t"; 
  out << "Drain "           << "(V)" << "\t";
  out << "Defect "          << "(%)" << "\t";
  out << "Trap "            << "(%)" << "\t";
  out << "ChargeGoal "      << "(%)" << "\t";
  out << "ChargeReached "   << "(%)" << "\t";
  out << "ChargeTransport " << "(per iteration)" << "\n";
  out.flush();

  for (int i = 0; i < input.steps(); ++i) {

    // Get simulation parameters for the current step and set up a new object
    input.simulationParameters(&par, i);

    // Set up a simulation
    Simulation sim(&par);

    // Charge the grid up is specified
    if (par.gridCharge) sim.seedCharges();

    // Open outputfile for this simulation
    QFile iterFile(outputFileName+"-i-"+QString::number(i)+".dat");
    if (!iterFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "Error opening output file:" << outputFileName+"-i-"+QString::number(i)+".dat";
      app.exit(1);
    }
    QTextStream iterOut(&iterFile);
   
    // Open trajectoryFile for this simulation
    QFile trajFile(outputFileName+"-i-"+QString::number(i)+".xyz");
    if (!trajFile.open(QIODevice::WriteOnly | QIODevice::Text)) {
      qDebug() << "Error opening trajectory file:" << outputFileName+"-i-"+QString::number(i)+".xyz";
      app.exit(1);
    }
    QTextStream trajOut(&trajFile);

    // Output Column Titles
    iterOut << "#i "              << "(n)" << "\t";
    iterOut << "Temperature "     << "(K)" << "\t";
    iterOut << "Source "          << "(V)" << "\t";
    iterOut << "Drain "           << "(V)" << "\t";
    iterOut << "Defect "          << "(%)" << "\t";
    iterOut << "Trap "            << "(%)" << "\t";
    iterOut << "ChargeGoal "      << "(%)" << "\t";
    iterOut << "ChargeReached "   << "(%)" << "\t";
    iterOut << "ChargeTransport " << "(per iteration)" << "\n";
 
    // Perform Warmup
    unsigned long lastCount = 0;
    for (int j = 0; j < par.iterationsWarmup; j += par.iterationsPrint) {

     // Perform Iterations
     sim.performIterations(par.iterationsPrint);

     // Output Trajectory
     if ( par.iterationsXYZ ) sim.getGrid()->print3D(trajOut);

     // Output Log
     qDebug() << j
              << "\t" << double(sim.charges()) / double(sim.getMaxCharges()) * 100.0
              << "\t" << double(sim.totalChargesAccepted()-lastCount) / par.iterationsPrint;
     iterOut  << j
              << "\t" << par.temperatureKelvin
              << "\t" << par.voltageSource
              << "\t" << par.voltageDrain
              << "\t" << par.defectPercentage * 100.00
              << "\t" << par.trapPercentage * 100.0
              << "\t" << par.chargePercentage * 100.0
              << "\t" << double(sim.charges()) / double(sim.getMaxCharges()) * 100.0
              << "\t" << double(sim.totalChargesAccepted()-lastCount) / par.iterationsPrint << "\n";
      iterOut.flush();
      lastCount = sim.totalChargesAccepted();
    }

    // Perform production
    unsigned long startCount = lastCount;
    for (int j = 0; j < par.iterationsReal; j += par.iterationsPrint) {

      // Perform Iterations
      sim.performIterations(par.iterationsPrint);

      // Output Trajectory
      if ( par.iterationsXYZ ) sim.getGrid()->print3D(trajOut);

      // Output Log
      qDebug() << j
               << "\t" << double(sim.charges()) / double(sim.getMaxCharges()) * 100.0
               << "\t" << double(sim.totalChargesAccepted()-lastCount) / par.iterationsPrint;
      iterOut  << j
               << "\t" << par.temperatureKelvin
               << "\t" << par.voltageSource
               << "\t" << par.voltageDrain
               << "\t" << par.defectPercentage * 100.0
               << "\t" << par.trapPercentage * 100.0
               << "\t" << par.chargePercentage * 100.0
               << "\t" << double(sim.charges()) / double(sim.getMaxCharges()) * 100.0
               << "\t" << double(sim.totalChargesAccepted()-lastCount) / par.iterationsPrint << "\n";
      iterOut.flush();
      lastCount = sim.totalChargesAccepted();
    }
    // Now to output the result of the simulation at this data point
    out << i
        << "\t" << par.temperatureKelvin
        << "\t" << par.voltageSource
        << "\t" << par.voltageDrain
        << "\t" << par.defectPercentage * 100.0
        << "\t" << par.trapPercentage * 100.0
        << "\t" << par.chargePercentage * 100.0
        << "\t" << double(sim.charges()) / double(sim.getMaxCharges()) * 100.0
        << "\t" << double(lastCount-startCount) / par.iterationsReal << "\n";
    out.flush();

    iterFile.close();

    if ( !par.iterationsXYZ ) 
     {
      trajFile.remove();
     }
    else
     {
      trajFile.close();
     }
  }

  outputFile.close();
  qDebug() << "Simulation complete - destroy our objects...";

}