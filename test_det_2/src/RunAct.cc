#include "RunAct.hh"
#include "iostream"
#include "map"
#include "CommandMessenger.hh"
#include "DetGeometry.hh"
#include "G4Timer.hh"

using namespace std;

RunAct::RunAct(DetGeometry*det,PrimaryGen*gen):G4UserRunAction(),fGen(gen),fDetector(det)

{
    result = new std::map<G4double, G4int>;
    result2 = new std::map<G4double, G4int>;
    runactMessenger = new RunActMessenger(this);
}

RunAct::~RunAct()
{
    delete result;
    delete result2;
    delete runactMessenger;
}

void RunAct::BeginOfRunAction(const G4Run *aRun)
{
    result->clear();
    for (int i=0; i <n_of_Bins; i++)
        result->insert(std::pair<G4double, G4int> (i *Emax / n_of_Bins, 0));

    result2->clear();
    for (int i=0; i <n_of_Bins; i++)
        result2->insert(std::pair<G4double, G4int> (i *Nmax / n_of_Bins, 0));

    if (isMaster)
    {
        Time = G4Timer().GetClockTime();
    }
}


void RunAct::EndOfRunAction(const G4Run *aRun)
{
    if(!isMaster)
    {
        static G4Mutex myMutex = G4MUTEX_INITIALIZER;
        G4MUTEXLOCK(&myMutex);

        static auto test = new double[n_of_Bins]();
        for (int i = 0; i < n_of_Bins; i++) {
            test[i] += result->at(i * Emax / n_of_Bins);
        }

        static auto test2 = new double[n_of_Bins]();

        for (int i = 0; i < n_of_Bins; i++) {
            test2[i] += result2->at(i * Nmax / n_of_Bins);
        }

        static double num = 0;
        num++;
        if (num == G4Threading::GetNumberOfRunningWorkerThreads()) {
            stringstream res_out_mutex;
            stringstream res_out_mutex2;
            stringstream res_out_mutex3;
            res_out_mutex << "../res/Deposited Energy.txt";
            res_out_mutex2 << "../res/Photons Counter.txt";
            res_out_mutex3 << "../res/Photons Counter Energy For Neutron " << fGen->U << " MeV.txt";

            std::ofstream myFile(res_out_mutex.str(), ios::out);
            for (int i = 0; i < n_of_Bins; i++) {
                myFile << i * Emax / n_of_Bins << '\t' << test[i] << endl;
                test[i] = 0;
            }

            std::ofstream myFile2(res_out_mutex2.str(), ios::out);
            for (int i = 0; i < n_of_Bins; i++) {
                myFile2 << i * Nmax / n_of_Bins << '\t' << test2[i] << endl;
            }

            std::ofstream myFile3(res_out_mutex3.str(), ios::out);
            for (int i = 0; i < n_of_Bins; i++) {
                myFile3 << i*2*9.871e-4 - 7.97e-5 << '\t' << test2[i] << endl;
                test2[i] = 0;
            }

            num = 0;
        }
        G4MUTEXUNLOCK(&myMutex);
    }

    if (isMaster)
    {
        auto P = new double *[n_of_Bins];
        for (int i = 0; i < n_of_Bins; i++) {
            P[i] = new double[n_of_Bins]();}

        auto Sigma = new double[n_of_Bins]();
        auto EnergyScale = new double[n_of_Bins]();
        auto Counts = new double[n_of_Bins]();
        auto Counts_norm = new double[n_of_Bins]();
        auto Counts_pre = new double[n_of_Bins]();

        double E, Cnt;

        double row_sum, nrm1, nrm2;
        row_sum = 0;
        nrm1 = 0;
        nrm2 = 0;

        ifstream read_1;
        read_1.open("../res/Deposited Energy.txt");
        for (int i = 0; i < n_of_Bins; i++) {
            read_1 >> E >> Cnt;
            EnergyScale[i] = E;
            Counts[i] = Cnt;
        }
        read_1.close();

        Counts[0] = 0;
        EnergyScale[0] = 0.000001;

        double delta = EnergyScale[10]-EnergyScale[9];

        for (int i=0; i<n_of_Bins; i++)
        {Sigma[i] = 2.55*(sqrt(13.1*3*EnergyScale[i]))/1000;}

        for (int i = 0; i < n_of_Bins; i++)
        {
            for (int j = 0; j < n_of_Bins; j++)
            {
                P[i][j] = (delta*Counts[j] / (sqrt(2 * pi * pow(Sigma[i],2)))) *
                          exp(-1*(pow((EnergyScale[i] - EnergyScale[j]),2) /
                                  (2 * pow(Sigma[i],2))));
            }
        }

        for (int i = 0; i < n_of_Bins; i++)
        {
            for (int j = 0; j < n_of_Bins; j++)
            {
                row_sum += P[i][j];
            }
            Counts_pre[i] = row_sum;
            row_sum = 0;
        }

        for (int i=0; i<n_of_Bins; i++)
        {
            nrm1+=delta*Counts[i];
            nrm2+=delta*Counts_pre[i];
        }

        for (int i=0; i<n_of_Bins; i++)
        {
            Counts_norm[i]=(nrm1/nrm2)*Counts_pre[i];
        }

        fstream fout_norm("../res/Deposited Energy Norm.txt", ios::out);
        for (int i = 0; i < n_of_Bins; i++)
        {
            if(Counts_norm[i]<1E-10)
            {Counts_norm[i]=0;}
            fout_norm << EnergyScale[i] << '\t' << Counts_norm[i] << '\n';
        }
        fout_norm.close();

        for (int i = 0; i < n_of_Bins; i++) { delete[] P[i]; }
        delete [] EnergyScale; delete [] Counts; delete [] Counts_norm;
        delete [] Counts_pre;  delete [] Sigma;

        G4cout << "Start time is: " << Time << endl;
        G4cout << "End time is: " << G4Timer().GetClockTime() << endl;
    }
}

void RunAct::AddEvent(G4double energy)
{
    auto it = result->lower_bound(energy);
    it->second++;
}

void RunAct::AddEvent2(G4double photons)
{
    auto it = result2->lower_bound(photons);
    it->second++;
}

void RunAct::SetNewResPath(G4String newPath) {

    foldername = newPath;

}

