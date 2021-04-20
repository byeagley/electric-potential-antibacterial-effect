/*
###############################################################################
# If you use PhysiCell in your project, please cite PhysiCell and the version #
# number, such as below:                                                      #
#                                                                             #
# We implemented and solved the model using PhysiCell (Version x.y.z) [1].    #
#                                                                             #
# [1] A Ghaffarizadeh, R Heiland, SH Friedman, SM Mumenthaler, and P Macklin, #
#     PhysiCell: an Open Source Physics-Based Cell Simulator for Multicellu-  #
#     lar Systems, PLoS Comput. Biol. 14(2): e1005991, 2018                   #
#     DOI: 10.1371/journal.pcbi.1005991                                       #
#                                                                             #
# See VERSION.txt or call get_PhysiCell_version() to get the current version  #
#     x.y.z. Call display_citations() to get detailed information on all cite-#
#     able software used in your PhysiCell application.                       #
#                                                                             #
# Because PhysiCell extensively uses BioFVM, we suggest you also cite BioFVM  #
#     as below:                                                               #
#                                                                             #
# We implemented and solved the model using PhysiCell (Version x.y.z) [1],    #
# with BioFVM [2] to solve the transport equations.                           #
#                                                                             #
# [1] A Ghaffarizadeh, R Heiland, SH Friedman, SM Mumenthaler, and P Macklin, #
#     PhysiCell: an Open Source Physics-Based Cell Simulator for Multicellu-  #
#     lar Systems, PLoS Comput. Biol. 14(2): e1005991, 2018                   #
#     DOI: 10.1371/journal.pcbi.1005991                                       #
#                                                                             #
# [2] A Ghaffarizadeh, SH Friedman, and P Macklin, BioFVM: an efficient para- #
#     llelized diffusive transport solver for 3-D biological simulations,     #
#     Bioinformatics 32(8): 1256-8, 2016. DOI: 10.1093/bioinformatics/btv730  #
#                                                                             #
###############################################################################
#                                                                             #
# BSD 3-Clause License (see https://opensource.org/licenses/BSD-3-Clause)     #
#                                                                             #
# Copyright (c) 2015-2021, Paul Macklin and the PhysiCell Project             #
# All rights reserved.                                                        #
#                                                                             #
# Redistribution and use in source and binary forms, with or without          #
# modification, are permitted provided that the following conditions are met: #
#                                                                             #
# 1. Redistributions of source code must retain the above copyright notice,   #
# this list of conditions and the following disclaimer.                       #
#                                                                             #
# 2. Redistributions in binary form must reproduce the above copyright        #
# notice, this list of conditions and the following disclaimer in the         #
# documentation and/or other materials provided with the distribution.        #
#                                                                             #
# 3. Neither the name of the copyright holder nor the names of its            #
# contributors may be used to endorse or promote products derived from this   #
# software without specific prior written permission.                         #
#                                                                             #
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" #
# AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE   #
# IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE  #
# ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE   #
# LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR         #
# CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF        #
# SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS    #
# INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN     #
# CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)     #
# ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE  #
# POSSIBILITY OF SUCH DAMAGE.                                                 #
#                                                                             #
###############################################################################
*/

#include "./custom.h"

void create_cell_types( void )
{
	// set the random seed 
	SeedRandom( parameters.ints("random_seed") );  
	
	/* 
	   Put any modifications to default cell definition here if you 
	   want to have "inherited" by other cell types. 
	   
	   This is a good place to set default functions. 
	*/ 
	
	initialize_default_cell_definition(); 
	cell_defaults.phenotype.secretion.sync_to_microenvironment( &microenvironment ); 
	
	cell_defaults.functions.volume_update_function = standard_volume_update_function;
	cell_defaults.functions.update_velocity = standard_update_cell_velocity;

	cell_defaults.functions.update_migration_bias = NULL; 
	cell_defaults.functions.update_phenotype = NULL; // update_cell_and_death_parameters_O2_based; 
	cell_defaults.functions.custom_cell_rule = NULL; 
	cell_defaults.functions.contact_function = NULL; 
	
	cell_defaults.functions.add_cell_basement_membrane_interactions = NULL; 
	cell_defaults.functions.calculate_distance_to_membrane = NULL; 
	
	/*
	   This parses the cell definitions in the XML config file. 
	*/
	
	initialize_cell_definitions_from_pugixml(); 
	
	/* 
	   Put any modifications to individual cell definitions here. 
	   
	   This is a good place to set custom functions. 
	*/ 
	
    
	cell_defaults.functions.update_phenotype = phenotype_function; 
	cell_defaults.functions.custom_cell_rule = custom_function; 
	cell_defaults.functions.contact_function = contact_function; 
	
    Cell_Definition* pPseudomonas = find_cell_definition( "Pseudomonas Aeruginosa" ); 
	Cell_Definition* pStaphylococcus = find_cell_definition( "Staphylococcus Aureus" ); 
	
	pPseudomonas->functions.update_phenotype = chemo_phenotype; 
	pStaphylococcus->functions.update_phenotype = chemo_phenotype; 
	
    
	/*
	   This builds the map of cell definitions and summarizes the setup. 
	*/
		
	build_cell_definitions_maps(); 
	display_cell_definitions( std::cout ); 
	
	return; 
}

void setup_microenvironment( void )
{
	// set domain parameters 
	
	// put any custom code to set non-homogeneous initial conditions or 
	// extra Dirichlet nodes here. 
	
	// initialize BioFVM 
	
	initialize_microenvironment(); 	
	
	return; 
}

void setup_tissue( void )
{
	double Xmin = microenvironment.mesh.bounding_box[0]; 
	double Ymin = microenvironment.mesh.bounding_box[1]; 
	double Zmin = microenvironment.mesh.bounding_box[2]; 

	double Xmax = microenvironment.mesh.bounding_box[3]; 
	double Ymax = microenvironment.mesh.bounding_box[4]; 
	double Zmax = microenvironment.mesh.bounding_box[5]; 
	
	if( default_microenvironment_options.simulate_2D == true )
	{
		Zmin = 0.0; 
		Zmax = 0.0; 
	}
	
	double Xrange = Xmax - Xmin; 
	double Yrange = Ymax - Ymin; 
	double Zrange = Zmax - Zmin; 
	
	// create some of each type of cell 
	
	Cell* pC;
	
	for( int k=0; k < cell_definitions_by_index.size() ; k++ )
	{
		Cell_Definition* pCD = cell_definitions_by_index[k]; 
		std::cout << "Placing cells of type " << pCD->name << " ... " << std::endl; 
		for( int n = 0 ; n < parameters.ints("number_of_cells") ; n++ )
		{
			std::vector<double> position = {0,0,0}; 
			position[0] = Xmin + UniformRandom()*Xrange; 
			position[1] = Ymin + UniformRandom()*Yrange; 
			position[2] = Zmin + UniformRandom()*Zrange; 
			
			pC = create_cell( *pCD ); 
			pC->assign_position( position );
		}
	}
	std::cout << std::endl; 
	
	// load cells from your CSV file (if enabled)
	load_cells_from_pugixml(); 	
	
	return; 
}

std::vector<std::string> my_coloring_function( Cell* pCell )
{ return paint_by_number_cell_coloring(pCell); }

void phenotype_function( Cell* pCell, Phenotype& phenotype, double dt )
{ return; }

void custom_function( Cell* pCell, Phenotype& phenotype , double dt )
{ return; } 

void contact_function( Cell* pMe, Phenotype& phenoMe , Cell* pOther, Phenotype& phenoOther , double dt )
{ return; } 

void chemo_phenotype( Cell* pCell, Phenotype& p , double dt)
{
	// sample environment 
	static int nQF1 = microenvironment.find_density_index( "quorum factor 1" ); 
	double c1 = pCell->nearest_density_vector()[nQF1]; 
	static int nQF2 = microenvironment.find_density_index( "quorum factor 2" ); 
	double c2 = pCell->nearest_density_vector()[nQF2]; 
    
	// Hill parameters 
	double hill = pCell->custom_data["Hill_coefficient"];  
	double c_half_max = pCell->custom_data["PD_half_max"]; 

    
	// apoptosis baseline and max 
	Cell_Definition* pCD = find_cell_definition( pCell->type ); 
	double base_apop = pCD->phenotype.death.rates[0]; 
	double max_apop = pCell->custom_data["PD_max_apoptosis"]; 
	
	// effect 
	double temp1_1 = pow(c1,hill); 
	double temp2_1 = pow(c_half_max,hill); 
	double E1 = temp1_1 / (temp1_1+temp2_1) * 0.5; 

	double temp1_2 = pow(c2,hill); 
	double temp2_2 = pow(c_half_max,hill); 
	double E2 = temp1_2 / (temp1_2+temp2_2) * 0.5; 

	static Cell_Definition* pPseudomonas= find_cell_definition( "Pseudomonas Aeruginosa"); 
	static Cell_Definition* pStaphylococcus = find_cell_definition( "Staphylococcus Aureus"); 

    // potential
    static int nPot = microenvironment.find_density_index( "electrical_potential" );
    double p1 = pCell->nearest_density_vector()[nPot];
    double pot_thr = pCell->custom_data["potential_threshold"];


	if(pCell->type_name == "Pseudomonas Aeruginosa")
	{
		pCell->phenotype.death.rates[0] = base_apop / (E1);
		if(pCell->phenotype.death.rates[0] > 0.1)
		{
			pCell->phenotype.death.rates[0] = 0.1;
		}

		pCell->phenotype.secretion.secretion_rates[1] = pCell->phenotype.secretion.secretion_rates[1] * E1;
        
        // secretion rate according to potential
        if ( p1 > pot_thr * -1)
        {
            if ( p1 < pot_thr)
            {
               pCell->phenotype.secretion.secretion_rates[1] *= 0.6;
               //std::cout << "PA sec rate: " << pCell->phenotype.secretion.secretion_rates[1] << std::endl;
            }
        }
	}

	if(pCell->type_name == "Staphylococcus Aureus")
	{
		pCell->phenotype.death.rates[0] = base_apop / (E2 / 5);
		if(pCell->phenotype.death.rates[0] > 0.5)
		{
			pCell->phenotype.death.rates[0] = 0.5;
		}

		pCell->phenotype.secretion.secretion_rates[1] = pCell->phenotype.secretion.secretion_rates[1] * E2; // VERIFY it is changing QF 1 
        
        // secretion rate according to potential
        if ( p1 > pot_thr * -1)
        {
            if ( p1 < pot_thr)
            {
                pCell->phenotype.secretion.secretion_rates[1] *= 0.6;
                //std::cout << "SA sec rate: " << pCell->phenotype.secretion.secretion_rates[1] << std::endl;
            }
        }
	}

	
	return; 
	}

void update_electrical_potential ( BioFVM::Microenvironment& Microenvironment, std::string filename )
{ 
    std::ifstream file( filename, std::ios::in );

	std::string line;
    int counter = 0;
    while (std::getline(file, line))
	{
		std::vector<double> data;
		csv_to_vector( line.c_str() , data ); 

        //std::cout << "TEST" << counter << std::endl;
		std::vector<double> position = { data[0]*1000 , data[1]*1000  , data[2]*1000 };

		double volt = (double) data[3]; 
        
        int voxel_ind = microenvironment.nearest_voxel_index ( position );
        
        microenvironment(voxel_ind)[0] = volt;
        
        //std::cout << microenvironment(voxel_ind)[1] << std::endl;
        counter += 1;
        std::cout << counter << std::endl;
	}
//	for( int n = 0; n < microenvironment.mesh.voxels.size() ; n++ )
	//{
      
		// csv or mat file input
      


        // std::vector<std::vector> data
        // data = csv.input() 
        
  //      microenvironment(n)[1] = 1.0;
        
		//std::cout << microenvironment.mesh.voxels.size() <<std::endl;
		
//	}
    



    return; 
}