#pragma once

#include <vector>
#include <math.h>

namespace uclv
{
    class Indicator
    {
        private:
            std::vector<double> xi_; //x coordinates of the taxels
            std::vector<double> yi_; //y coordinates of the taxels
            double Ix_ = 0.0; //x component of the indicator
            double Iy_ = 0.0; //y component of the indicator
            double I_ = 0.0; //module of the indicator
            double xc, xc0;
            double yc, yc0;
            double den, numx, numy;
            int taxel_number; //number of the tactile sensing elements
            int num_rows; //number of elements in a taxels matrix row
            int num_cols; //number of elements in a taxels matrix column
            double sp_res = 3.55; //spatial resolution: the distance between a taxel and the next one in millimeters
            double p = 1.775; // half of the spatial resolution

            // Compute yi
            double compute_yi_value(int j, int n, bool even)
            {
                double value;
                if (even)
                {
                    if(j<n/2)
                    {
                        value = (n/2 - (j+1))*sp_res + p;
                    }
                    else
                    {
                        value = (n/2 - j)*sp_res - p;
                    }
                }
                else
                {
                    value = ((n/2+1) - (j+1))*sp_res;
                }
                return value;
            }

            // Compute xi
            double compute_xi_value(int i, int n, bool even)
            {
                double value;
                if(even)
                {
                    if(i<n/2)
                    {
                        value = -(n/2 - (i+1))*sp_res - p;
                    }
                    else
                    {
                        value = -(n/2 - i)*sp_res + p;
                    }
                }
                else
                {
                    value = -((n/2+1) - (i+1))*sp_res;
                }
                return value;
            }

        public:
            //Constructor. Number of rows and columns of the tactile matrix as arguments
            Indicator(const int &num_r, const int &num_c) : num_rows(num_r), num_cols(num_c)
            {
                xc0 = 0.0;
                yc0 = 0.0;

                //Construct xi and yi
                xi_.clear();
                yi_.clear();
                taxel_number = num_rows*num_cols;

                bool num_rows_even;
                bool num_cols_even;

                //Check the number of rows and columns are even or odd
                if (num_cols%2)
                    num_cols_even = false;
                else
                    num_cols_even = true;

                if (num_rows%2)
                    num_rows_even = false;
                else
                    num_rows_even = true;

                //assign xi, yi
                for(int j=0; j<num_rows; j++)
                {
                    double y_row = compute_yi_value(j, num_rows, num_rows_even);
                    for(int i=0; i<num_cols; i++)
                    {
                        yi_.push_back(y_row);
                        xi_.push_back(compute_xi_value(i, num_cols, num_cols_even));
                    }                
                }

            }

            // Get the x component of the indicator
            const double &Ix() const
            {
                return Ix_;
            }
            
            // Get the y component of the indicator
            const double &Iy() const
            {
                return Iy_;
            }
            
            // Get the module of the indicator
            const double &I() const
            {
                return I_;
            }

            // Get the vector xi. It contains the x coordinates of the taxels
            const std::vector<double> &xi() const
            {
                return xi_;
            }
            
            // Get the vector yi. It contains the y coordinates of the taxels
            const std::vector<double> &yi() const
            {
                return yi_;
            }

            // Compute the indicator. Tactile data as input argument
            void compute_indicator(const std::vector<float> &data_read)
            {
                double den = 0.0;
                double numx = 0.0;
                double numy = 0.0;
                for (int i = 0; i < taxel_number; i++)
                {
                    den = den + data_read.at(i);
                    numx = numx + data_read.at(i) * xi_[i];
                    numy = numy + data_read.at(i) * yi_[i];
                }
                xc = numx / den;
                yc = numy / den;
                Ix_ = xc - xc0;
                Iy_ = yc - yc0;
                I_ = std::sqrt(Ix_ * Ix_ + Iy_ * Iy_);
            }

            // Reset the indicator
            void zero_indicator()
            {
                xc0 = xc;
                yc0 =  yc;
            }
    };
}