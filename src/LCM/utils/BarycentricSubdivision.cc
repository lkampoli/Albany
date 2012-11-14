//*****************************************************************//
//    Albany 2.0:  Copyright 2012 Sandia Corporation               //
//    This Software is released under the BSD license detailed     //
//    in the file "license.txt" in the top-level Albany directory  //
//*****************************************************************//

// Define only if ALbany is enabled
#if defined (ALBANY_LCM)

#include <stk_mesh/base/FieldData.hpp>
#include "Topology.h"

namespace LCM {


  //
  // \brief Determine highest id number for each entity rank.
  // Used to assign unique ids to newly created entities
  //
  void topology::set_highest_ids()
  {
    // Get space dimension by querying the STK discretization.
    Albany::STKDiscretization &
    stk_discretization =
        static_cast<Albany::STKDiscretization &>(*discretization_ptr_);

    const unsigned int number_dimensions =
        stk_discretization.getSTKMeshStruct()->numDim;

    highest_ids_.resize(number_dimensions);

    for (unsigned int rank = 0; rank < number_dimensions; ++rank) {
      highest_ids_[rank] = get_number_entities_by_rank(*bulkData_, rank);
    }

    return;
  }

  //
  // \brief Adds a new entity of rank 3 to the mesh
  //
  void
  topology::add_element(EntityRank entity_rank)
  {

      stk::mesh::PartVector part_vector(1);

      part_vector[0] = stkMeshStruct_->partVec[0];

      const unsigned int entity_id = ++highest_ids_[entity_rank];

     bulkData_->declare_entity(entity_rank,
          entity_id, part_vector);

	return;
  }

  /*
   * \brief creates several entities at a time. The information about
   * the type of entity and and the amount of entities is contained
   * in the input vector called: "Requests"
   */
   void
   topology::add_entities(std::vector<size_t> & Requests )
   {
       stk::mesh::EntityVector newEntity;
       bulkData_->generate_new_entities(Requests,newEntity);
       return;
   }


  //
  // \brief Removes an entity and all its connections
  //
  void topology::remove_entity(Entity & entity)
  {
	//Destroy all relations to or from the entity
	Entity * entities = &entity;
    stk::mesh::PairIterRelation relations = entity.relations();
	stk::mesh::PairIterRelation::iterator iterator_entity_relations;

	for(iterator_entity_relations = relations.begin();
		iterator_entity_relations != relations.end();++iterator_entity_relations){
        EdgeId edgeId = iterator_entity_relations->identifier();
        Entity & target = *(iterator_entity_relations->entity());
        bulkData_->destroy_relation(entity, target, edgeId);
    }
	// remove the entity from stk mesh
	bool deleted = bulkData_->destroy_entity(entities);
	assert(deleted);
	return;
   }

  //
  // \brief Adds a relation between two entities
  //
  void topology::add_relation(Entity & source_entity, Entity & target_entity,
      EdgeId local_relation_id)
  {
    bulkData_->declare_relation(source_entity, target_entity,
        local_relation_id);
    return;
  }

//
// \brief Removes the relation between two entities
//
  void topology::remove_relation(Entity & source_entity, Entity & target_entity,
      EdgeId local_relation_id)
  {
    bulkData_->destroy_relation(source_entity, target_entity,
        local_relation_id);
    return;
  }

//
// \brief Returns a vector with all the actual mesh entities of a specific rank
//
  std::vector<Entity*> topology::get_entities_by_rank(
      const stk::mesh::BulkData & mesh, EntityRank entity_rank)
  {
    std::vector<Entity*> entities;
    const std::vector<stk::mesh::Bucket*> & ks = mesh.buckets(entity_rank);
    entities.clear();
    size_t count = 0;
    const std::vector<stk::mesh::Bucket*>::const_iterator ie = ks.end();
    std::vector<stk::mesh::Bucket*>::const_iterator ik = ks.begin();

    for (; ik != ie; ++ik) {
      count += (*ik)->size();
    }
    entities.reserve(count);

    ik = ks.begin();
    for (; ik != ie; ++ik) {
      const stk::mesh::Bucket & k = **ik;
      size_t n = k.size();
      for (size_t i = 0; i < n; ++i) {
        entities.push_back(&k[i]);
      }
    }
    return entities;
  }

  //
  // \brief This returns the  number of entities on the former mesh of a given rank
  //
  std::vector<Entity*>::size_type topology::get_number_entities_by_rank(
      const stk::mesh::BulkData & mesh, EntityRank entity_rank)
  {
    return mesh.buckets(entity_rank).size();
  }

//
// \brief Gets the local relation id (0,1,2,...) between two entities
//
  EdgeId topology::get_local_relation_id(const Entity & source_entity,
      const Entity & target_entity)
  {

    EdgeId local_id;
    const stk::mesh::PairIterRelation &source_relations = source_entity.relations();
    unsigned int target_entity_identifier = target_entity.identifier();
    unsigned int target_entity_entity_rank = target_entity.entity_rank();

    stk::mesh::PairIterRelation::iterator iterator_source_relations;
    for (iterator_source_relations = source_relations.begin(); iterator_source_relations!=source_relations.end(); iterator_source_relations++) {
    	const Entity *entity = iterator_source_relations->entity();
    	if (entity->identifier()
          == target_entity_identifier
          && entity->entity_rank()
              == target_entity_entity_rank) {
        local_id = iterator_source_relations->identifier();
      }
    }
    return local_id;
  }

//
// \brief Returns the total number of lower rank entities connected to a specific entity
//
  unsigned int topology::get_number_lower_rank_entities(const Entity & entity)
  {

    unsigned int count = 0;
    const stk::mesh::PairIterRelation &entity_relations = entity.relations();
    unsigned int entity_rank=entity.entity_rank();

    stk::mesh::PairIterRelation::iterator iterator_relations;
    for(iterator_relations=entity_relations.begin(); iterator_relations!=entity_relations.end(); iterator_relations++){
    	if(entity_rank > iterator_relations->entity()->entity_rank()) count++;
    }

    return count;
  }

  /*
   * \brief Returns a group of entities connected directly to a given entity. The input rank is the rank
   *  of the returned entities.
   */
  std::vector<Entity*> topology::get_directly_connected_entities(
      const Entity & entity, EntityRank entity_rank)
  {
    std::vector<Entity*> returned_entities;
    const stk::mesh::PairIterRelation &entity_relations = entity.relations();
    stk::mesh::PairIterRelation::iterator iterator_relations;

    for(iterator_relations=entity_relations.begin(); iterator_relations!=entity_relations.end(); iterator_relations++){
    	if(iterator_relations->entity_rank() == entity_rank){
    		returned_entities.push_back(iterator_relations->entity());
    	}
    }
    return returned_entities;
  }

//
// \brief Checks if an entity exists inside a specific vector. returns "0" for true and "1" for false
//
  unsigned int topology::find_entity_in_vector(std::vector<Entity*> & entities,
      Entity * entity)
  {
    std::vector<Entity*>::iterator iterator_entities;
    unsigned int number = 1;
    for (iterator_entities = entities.begin();
        iterator_entities != entities.end(); ++iterator_entities) {
      if (*iterator_entities == entity) {
        number = 0;
        break;
      }
    }
    return number;
  }

  /*
   *  \brief Returns a group of entities connected indirectly to a given entity.
   *  e.g. of returns: nodes  that belong to a face
   *  segments or nodes that belong to an  element
   *  The input rank is the rank of the returned entities.
   *  The input rank must be lower than that of the input entity
   *
   */
  std::vector<Entity*> topology::get_boundary_entities(const Entity & entity,
      EntityRank entity_rank)
  {

    EntityRank given_entity_rank = entity.entity_rank();
    //Get entities of  "given_entity_rank -1"
    std::vector<std::vector<Entity*> > boundary_entities(given_entity_rank + 1);
    boundary_entities[given_entity_rank - 1] = get_directly_connected_entities(
        entity, given_entity_rank - 1);
    std::vector<Entity*>::iterator iterator_entities1;
    std::vector<Entity*>::iterator iterator_entities2;
    std::vector<Entity*> temp_vector1;
    for (unsigned int ii = given_entity_rank - 1; ii > entity_rank; ii--) {
      for (iterator_entities1 = boundary_entities[ii].begin();
          iterator_entities1 != boundary_entities[ii].end();
          ++iterator_entities1) {
        temp_vector1 = get_directly_connected_entities(*(*iterator_entities1),
            ii - 1);
        for (iterator_entities2 = temp_vector1.begin();
            iterator_entities2 != temp_vector1.end(); ++iterator_entities2) {
          if (find_entity_in_vector(boundary_entities[ii - 1],
              *iterator_entities2) == 1) {
            boundary_entities[ii - 1].push_back(*iterator_entities2);
          }
        }
        temp_vector1.clear();
      }
    }
    return boundary_entities[entity_rank];
  }

//
// \brief Checks if a segment is connected to an input node. Returns true "0" or false "1"
//
  unsigned int topology::check_segment_connection(const Entity & segment,
      Entity * node)
  {

    int number = 1;
    std::vector<Entity*> segment_nodes = get_boundary_entities(segment, 0);
    std::vector<Entity*>::iterator Iterator_nodes;
    for (Iterator_nodes = segment_nodes.begin();
        Iterator_nodes != segment_nodes.end(); ++Iterator_nodes) {
      if (*Iterator_nodes == node) {
        number = 0;
      }
    }
    return number;
  }

  /*
   * \brief Finds the adjacent segments to a given segment. The adjacent segments are connected to a given common point.
   * it returns  adjacent segments
   */
  std::vector<Entity*> topology::find_adjacent_segments(const Entity & segment,
      Entity * node)
  {

    std::vector<Entity*>::iterator Iterator_seg_nodes;
    std::vector<Entity*>::iterator Iterator_adj_nodes;
    std::vector<Entity*>::iterator Iterator_adj_seg;
    std::vector<Entity*> adjacent_segments;
    std::vector<Entity*> adjacent_segments_final;

    //Obtain the nodes corresponding to the input segment
    std::vector<Entity*> input_segment_nodes = get_directly_connected_entities(
        segment, 0);
    //Find the segments connected to "input_segment_nodes"
    for (Iterator_adj_nodes = input_segment_nodes.begin();
        Iterator_adj_nodes != input_segment_nodes.end(); ++Iterator_adj_nodes) {
      adjacent_segments = get_directly_connected_entities(
          *(*Iterator_adj_nodes), 1);
      //Which segment is connected to the input node?
      for (Iterator_adj_seg = adjacent_segments.begin();
          Iterator_adj_seg != adjacent_segments.end(); ++Iterator_adj_seg) {
        if (check_segment_connection(*(*Iterator_adj_seg), node) == 0) {
          adjacent_segments_final.push_back(*Iterator_adj_seg);
        }
      }
    }
    return adjacent_segments_final;
  }

//
// \brief Returns all the 3D entities connected to a given face
//
  std::vector<Entity*> topology::find_3D_relations(const Entity & face)
  {
	std::vector<Entity*> entities_3D;
    const stk::mesh::PairIterRelation & _relations = face.relations();
    stk::mesh::PairIterRelation::iterator iterator_relations;

    for (iterator_relations = _relations.begin();
	     iterator_relations != _relations.end(); ++iterator_relations){
		 if (iterator_relations->entity()->entity_rank() == 3) {
		        entities_3D.push_back(iterator_relations->entity());
	      }
	}
	return entities_3D;

  }

  /*
   * \brief Returns all the segments at the boundary of a given element. Including those
   * connected between the faces barycenters and the faces boundary nodes
   */
  std::vector<Entity*> topology::find_segments_from_element(const Entity & element)
  {
    std::vector<Entity*> element_faces;
    std::vector<Entity*> element_node;
    std::vector<Entity*> node_segments;
    std::vector<Entity*> _segments;
    std::vector<Entity*> outer_segments;
    std::vector<Entity*>::const_iterator iterator_element_faces;
    std::vector<Entity*>::const_iterator iterator_node_segments;
    std::vector<Entity*>::const_iterator iterator_outer_segments;

    element_faces = get_boundary_entities(element, 2);
    for (iterator_element_faces = element_faces.begin();
        iterator_element_faces != element_faces.end();
        ++iterator_element_faces) {
      element_node = get_directly_connected_entities(*(*iterator_element_faces),
          0);
      node_segments = get_directly_connected_entities(*(element_node[0]), 1);
      for (iterator_node_segments = node_segments.begin();
          iterator_node_segments != node_segments.end();
          ++iterator_node_segments) {
        _segments.push_back(*iterator_node_segments);
      }
    }
    outer_segments = get_boundary_entities(element, 1);
    for (iterator_outer_segments = outer_segments.begin();
        iterator_outer_segments != outer_segments.end();
        ++iterator_outer_segments) {
      _segments.push_back(*iterator_outer_segments);
    }
    return _segments;
  }


  /*
   * \brief Returns "0" if the input faces have two points in common. Otherwise,
   * it returns "1"
   */
  bool topology::compare_faces(const Entity & face1, const Entity & face2)
  {
	 std::vector<Entity*> face1_nodes;
	 std::vector<Entity*> face2_nodes;
	 std::vector<Entity*> common_nodes;
	 std::vector<Entity*>::iterator iterator_entity_faces;

	face1_nodes = get_boundary_entities(face1, 0);
	face2_nodes = get_boundary_entities(face2, 0);
	 bool num = true;
	 for(iterator_entity_faces = face2_nodes.begin();
		iterator_entity_faces != face2_nodes.end();++iterator_entity_faces){
		 if (find_entity_in_vector(face1_nodes, *iterator_entity_faces) == 0) {
		         common_nodes.push_back(*iterator_entity_faces);
         }
	 }
	 if (common_nodes.size() == 2) {
	        num = false;
		}
     return num;
  }


 // \brief returns the adjacent segments from a given face
 std::vector<Entity*>
 topology::find_adjacent_faces_from_face(
 const std::vector<std::vector<Entity*> > & faces_inside_element, const Entity & _face,
 const int element_number){
 std::vector<Entity*> adjacent_faces;
 std::vector<Entity*>::const_iterator iterator_element_internal_faces;
 std::vector<Entity*> _element_internal_faces = faces_inside_element[element_number];

 for (iterator_element_internal_faces = _element_internal_faces.begin();
	   iterator_element_internal_faces != _element_internal_faces.end();
	   ++iterator_element_internal_faces) {
	   if (compare_faces(_face, *(*iterator_element_internal_faces)) == 0) {
	          adjacent_faces.push_back(*iterator_element_internal_faces);
	   }
    }
   return adjacent_faces;
 }


//
// \brief Returns a pointer with the coordinates of a given entity
//
  double*
  topology::get_pointer_of_coordintes(Entity * entity)
  {

    Teuchos::RCP<Albany::AbstractDiscretization> discretization_ptr =
        get_Discretization();
    Albany::STKDiscretization & stk_discretization =
        static_cast<Albany::STKDiscretization &>(*discretization_ptr);
    //Obtain the stkMeshStruct
    Teuchos::RCP<Albany::AbstractSTKMeshStruct> stkMeshStruct =
        stk_discretization.getSTKMeshStruct();
    //Create the pointer of coordinates
    double* pointer_coordinates = stk::mesh::field_data(
        *stkMeshStruct->coordinates_field, *entity);

    return pointer_coordinates;
  }

//
// brief Returns a vector with the corresponding former boundary nodes of an input entity of rank 3
//

  std::vector<Entity*> topology::get_former_element_nodes(const Entity & element,
     const std::vector<std::vector<Entity*> > & entities)
  {
	std::vector<Entity*> vector_nodes_;
	std::vector<Entity*> _boundary_nodes;
	std::vector<Entity*>::iterator iterator_nodes;
	vector_nodes_ = entities[element.identifier()];

	for (iterator_nodes = vector_nodes_.begin();
		iterator_nodes != vector_nodes_.end();++iterator_nodes){
	      _boundary_nodes.push_back(*iterator_nodes);
	}
    return _boundary_nodes;

  }

  /*
   * brief Generates the coordinate of a given barycenter
   * "entities" is a vector with the entities of rank "0" that belong to the same higher rank entity
   *  connected to the barycenter(e.g segment, face, or element)
   */
  void
  topology::create_coordinates(
     const std::vector<Entity*> & entities, Entity * barycenter)
  {

    //vector of pointers
    std::vector<double*> vector_pointers;
    std::vector<Entity*>::const_iterator iterator_entities;
    //Copy all the fields from entity1 to the new middle node called "barycenter"
    stk::mesh::BulkData* bulkData_ = get_BulkData();
    bulkData_->copy_entity_fields(*entities[0], *barycenter);

    //With the barycenter coordinate initialized, take the average between the entities that belong to
    //the vector called: "entities"
    for (iterator_entities = entities.begin();iterator_entities != entities.end();
    	++iterator_entities){
    	vector_pointers.push_back(get_pointer_of_coordintes(*iterator_entities));
    }

    //Pointer with coordinates without average
    std::vector<double> coordinates_(3);
    for (unsigned int ii = 0; ii < vector_pointers.size(); ++ii) {
      coordinates_[0] += vector_pointers[ii][0];
      coordinates_[1] += vector_pointers[ii][1];
      coordinates_[2] += vector_pointers[ii][2];
    }

    //Pointer with the barycenter coordinates
    double* barycenter_coordinates = get_pointer_of_coordintes(barycenter);
    barycenter_coordinates[0] = coordinates_[0] / (1.0 * entities.size());
    barycenter_coordinates[1] = coordinates_[1] / (1.0 * entities.size());
    barycenter_coordinates[2] = coordinates_[2] / (1.0 * entities.size());

    return;
  }


// \brief Barycentric subdivision of simplicial meshes
//
  void topology::barycentric_subdivision()
  {
    // Use to assign unique ids
    set_highest_ids();

    // Begin mesh update
    bulkData_->modification_begin();

//-----------------------------------------------------------------------------------------------------------------------------------
// I. Divide all the segments of the mesh by half
// initial_entities_0D: Vector with all the entities of rank "0" (nodes) required to divide the original mesh segments by half
// initial_entities_1D: Vector that contains all the entities of rank "1" (segments) of the original mesh
// modified1_entities_1D: Vector with all the segments required to divide the original mesh segments by half
// initial_entities_3D: vector with all the elements of the former mesh
// Assign coordinates to the new nodes needed to divide the segments by half
//------------------------------------------------------------------------------------------------------------------------------------

    //MEASURING TIME
     clock_t start1, end1;
     double cpu_time_used1;
     start1 = clock();

    //Get the segments from the original mesh
    std::vector<Entity*> initial_entities_1D = get_entities_by_rank(
        *(bulkData_), 1);
    std::vector<Entity*> vector_nodes;

    //Adding nodes to divide segments by half
    std::vector<size_t> requests1(number_dimensions_ + 1, 0);
    requests1[0] = initial_entities_1D.size();
    std::vector<size_t> requests_step1_1(number_dimensions_ + 1, 0);
    requests_step1_1[0] = initial_entities_1D.size();
    add_entities(requests_step1_1);

    std::vector<Entity*> initial_entities_0D = get_entities_by_rank(
        *(bulkData_), 0);

    //vector with all elements from former mesh. This is used in step VI
    std::vector<Entity*> initial_entities_3D = get_entities_by_rank(
        *(bulkData_), 3);
    //Create a vector of vectors that contains all the former boundary nodes of all the elements of the mesh
    std::vector<std::vector<Entity*> > all_elements_boundary_nodes1(
        initial_entities_3D.size() + 1);
    //temporary vector //check the values inside this vector
    for (unsigned int ii = 0; ii < initial_entities_3D.size(); ++ii) {
      all_elements_boundary_nodes1[ii + 1] = get_boundary_entities(
          *(initial_entities_3D[ii]), 0);
    }

    for (unsigned int ii = 0; ii < initial_entities_1D.size(); ++ii) {
      //Create a vector with all the initial nodes connected to a segment
      vector_nodes = get_directly_connected_entities(*(initial_entities_1D[ii]),
          0);
      //Look for all the relations of each segment
      stk::mesh::PairIterRelation _relations =
          initial_entities_1D[ii]->relations();
      for (unsigned int i = 0; i < _relations.size(); ++i) {
        if (_relations[i].entity()->entity_rank() == 0
            && get_local_relation_id(*(initial_entities_1D[ii]),
                *(_relations[i].entity())) == 1) {
          //Add a blue(local relation) connection. This is only for reference
        	add_relation(*(initial_entities_1D[ii]), *(_relations[i].entity()),
              2);
          //Remove the relation between the former segment and node
          remove_relation(*(initial_entities_1D[ii]), *(_relations[i].entity()),
              1);
          //Add a relation from the former segment to a new node
          add_relation(*(initial_entities_1D[ii]), *(initial_entities_0D[ii]),
              1);
        }
      }
      //Assign coordinates to the new nodes
      create_coordinates(vector_nodes, initial_entities_0D[ii]);
    }


    //Adding segments
    std::vector<size_t> requests_step1_2(number_dimensions_ + 1, 0);
    requests_step1_2[1] = initial_entities_1D.size();
    add_entities(requests_step1_2);
    std::vector<Entity*> modified1_entities_1D = get_entities_by_rank(
        *(bulkData_), 1);

    for (unsigned int ii = 0; ii < initial_entities_1D.size(); ++ii) {
      //Look for all the relations of each segment
      stk::mesh::PairIterRelation _relations =
          initial_entities_1D[ii]->relations();
      for (unsigned int i = 0; i < _relations.size(); ++i) {
        if (_relations[i].entity()->entity_rank() == 0
            && get_local_relation_id(*(initial_entities_1D[ii]),
                *(_relations[i].entity())) == 2) {
          //Add a relation between the new segment and a node
          add_relation(*(modified1_entities_1D[ii]), *(_relations[i].entity()),
              0);
          //Remove this connection. This was only for reference
             remove_relation(*(initial_entities_1D[ii]), *(_relations[i].entity()),
              2);
          //Add a relation between the new segment and the "middle" node
          add_relation(*(modified1_entities_1D[ii]), *(initial_entities_0D[ii]),
              1);
        }
      }
    }

    //Adding the new segments to its corresponding faces
    //The segments can be connected to 1 one or more faces
    for (unsigned int ii = 0; ii < initial_entities_1D.size(); ++ii) {
      stk::mesh::PairIterRelation _relations =
          initial_entities_1D[ii]->relations();
      for (unsigned int i = 0; i < _relations.size(); ++i) {
        if (_relations[i].entity()->entity_rank() == 2) {
          add_relation(*(_relations[i].entity()), *(modified1_entities_1D[ii]),
              get_number_lower_rank_entities(*(_relations[i].entity())));
        }
      }
    }

    //Get the former faces from the mesh
    std::vector<Entity*> initial_entities_2D = get_entities_by_rank(
        *(bulkData_), 2);
    //Calculate the final number of segments per face after the division of the segments
    const stk::mesh::PairIterRelation & _relations =
            initial_entities_2D[0]->relations();
    stk::mesh::PairIterRelation::iterator iterator_Relations_;
    std::vector<Entity*> segments;
    for (iterator_Relations_ = _relations.begin();
    	iterator_Relations_ != _relations.end();++iterator_Relations_){
    	if (iterator_Relations_->entity()->entity_rank() == 1) {
    	        segments.push_back(iterator_Relations_->entity());
    	}
    }

    //Number of segments per face after division by half
    unsigned int Num_segments_face = segments.size();

    //MEASURING TIME
    end1 = clock();
    cpu_time_used1 = ((double) (end1 - start1)) / CLOCKS_PER_SEC;
    std::cout << endl;
    std::cout << "First part takes "
   		  << cpu_time_used1 << " seconds"<<endl;
//-----------------------------------------------------------------------------------------------------------------------------------
// II.Connect the new center nodes to the center of the face
// mofified1_entities_0D: Vector of nodes that includes all the ones up the "node centers of the faces"
// initial_entities_2D: Vector with the faces of the original mesh
// Add the corresponding coordinates to the barycenters of all faces
//------------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
     clock_t start2, end2;
     double cpu_time_used2;
     start2 = clock();
    //Adding new nodes to the centers of the faces of the original mesh
    std::vector<size_t> requests_step2(number_dimensions_ + 1, 0);
    requests_step2[0] = initial_entities_2D.size();
    add_entities(requests_step2);

    std::vector<Entity*> modified1_entities_0D = get_entities_by_rank(
        *(bulkData_), 0);

    for (unsigned int ii = 0; ii < initial_entities_2D.size(); ++ii) {
      //Connect the node to its corresponding face
      add_relation(*(initial_entities_2D[ii]), *(modified1_entities_0D[ii]),
          get_number_lower_rank_entities(*(initial_entities_2D[ii])));
    }

    //Add the corresponding coordinates to the barycenters of all faces
    std::vector<Entity*> boundary_nodes;
    for (unsigned int ii = 0; ii < initial_entities_2D.size(); ++ii) {
      boundary_nodes = get_boundary_entities(*(initial_entities_2D[ii]), 0);
      create_coordinates(boundary_nodes, modified1_entities_0D[ii]);
    }
    //MEASURING TIME
    end2 = clock();
    cpu_time_used2 = ((double) (end2 - start2)) / CLOCKS_PER_SEC;
    std::cout << endl;
      std::cout << "The second part takes "
     		  << cpu_time_used2 << " seconds"<<endl;
//-----------------------------------------------------------------------------------------------------------------------------------
// III. For each face start creating new segments that will connect the center point
// of the face with with all the points at its boundary
// modified2_entities_1D: Vector with all the segments up the new ones defined in III.
// vector_boundary_points: Vector with all the boundary nodes of all faces of the element
// New_Boundary_segments: Number of new segments that connect the
// centroid of each face  with the points at the face's boundary
//------------------------------------------------------------------------------------------------------------------------------------

    //MEASURING TIME
    clock_t start3, end3;
    double cpu_time_used3;
    start3 = clock();
    //Add the new segments that will connect the center point with the points at the boundary
    const int New_Boundary_segments =  (get_directly_connected_entities(
		   *initial_entities_2D[0],1).size())*(initial_entities_2D.size());
    std::vector<size_t> requests_step3(number_dimensions_ + 1, 0);
    requests_step3[1] = New_Boundary_segments;
    add_entities(requests_step3);

    //Vector that contains the latest addition of segments
    std::vector<Entity*> modified2_entities_1D = get_entities_by_rank(
        *(bulkData_), 1);

    //Vector with all the boundary nodes of all faces of the element
    std::vector<Entity*>::iterator iterator_entities1;
    std::vector<Entity*>::iterator iterator_entities2;
    std::vector<Entity*> vector_boundary_points1;
    std::vector<Entity*> vector_boundary_points;
    for (iterator_entities1 = initial_entities_2D.begin();
        iterator_entities1 != initial_entities_2D.end(); ++iterator_entities1) {
      //Create a vector with all the "0 rank" boundaries (nodes)
      vector_boundary_points1 = get_boundary_entities(*(*iterator_entities1),
          0);
      //Push all the vector of boundary points into a single one
      for (iterator_entities2 = vector_boundary_points1.begin();
          iterator_entities2 != vector_boundary_points1.end();
          ++iterator_entities2) {
        vector_boundary_points.push_back(*iterator_entities2);
      }
      vector_boundary_points1.clear();
    }

    //Connect the new segments to the corresponding nodes
    for (unsigned int ii = 0; ii < Num_segments_face * initial_entities_2D.size();
        ++ii) {
      //Add a relation between the segments and the center nodes
      add_relation(*(modified2_entities_1D[ii]),
          *(modified1_entities_0D[ii / Num_segments_face]), 0);
      //Add a relation between the segments and the boundary nodes
      add_relation(*(modified2_entities_1D[ii]), *(vector_boundary_points[ii]),
          1);
    }

    //Create a vector with all the boundary segments of the elements
    // "All_boundary_segments" and "Number_new_triangles_inside_element" used in step VIII.
    int Number_new_triangles_inside_element;
    std::vector<Entity*> All_boundary_segments;
    std::vector<Entity*> element_segments;
    std::vector<Entity*>::iterator iterator_elements_;
    std::vector<Entity*>::iterator iterator_element_segments;
    for (iterator_elements_ = initial_entities_3D.begin();
        iterator_elements_ != initial_entities_3D.end(); ++iterator_elements_) {
      element_segments = find_segments_from_element(*(*iterator_elements_));
      Number_new_triangles_inside_element = element_segments.size();
      for (iterator_element_segments = element_segments.begin();
          iterator_element_segments != element_segments.end();
          ++iterator_element_segments) {
        All_boundary_segments.push_back(*iterator_element_segments);
      }
    }
    //MEASURING TIME
    end3 = clock();
    cpu_time_used3 = ((double) (end3 - start3)) / CLOCKS_PER_SEC;
    std::cout << endl;
    std::cout << "The Third part takes "
       		  << cpu_time_used3 << " seconds"<<endl;
//-----------------------------------------------------------------------------------------------------------------------------------
// IV. Define the new faces at the boundary of the elements
// modified1_entities_2D: Vector that contains all the faces up to the new ones at the boundary of the elements
// "all_faces_centroids" and "All_boundary_segments" defined below
//-----------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
    clock_t start4, end4;
    double cpu_time_used4;
    start4 = clock();
   //Add the new faces
   std::vector<size_t> requests_step4(number_dimensions_ + 1, 0);
   requests_step4[2] = Num_segments_face * initial_entities_2D.size();;
   add_entities(requests_step4);

    std::vector<Entity*> modified1_entities_2D = get_entities_by_rank(
        *(bulkData_), 2);

    //iterators
    std::vector<Entity*>::iterator iterator_faces;
    std::vector<Entity*>::iterator iterator_segments;
    std::vector<Entity*>::iterator iterator_nodes;
    //vectors
    std::vector<Entity*> vector_segments;
    std::vector<Entity*> face_centroid;
    std::vector<Entity*> all_boundary_segments;
    std::vector<Entity*> all_faces_centroids;

    //Put all the boundary segments in a single vector. Likewise, the nodes.
    for (iterator_faces = initial_entities_2D.begin();
        iterator_faces != initial_entities_2D.end(); ++iterator_faces) {
      //vector_segments, This vector contains the boundary segments that conform a specific face
      vector_segments = get_directly_connected_entities(*(*iterator_faces), 1);
      face_centroid = get_directly_connected_entities(*(*iterator_faces), 0);
      for (iterator_segments = vector_segments.begin();
          iterator_segments != vector_segments.end(); ++iterator_segments) {
        all_boundary_segments.push_back(*iterator_segments);
      }
      for (iterator_nodes = face_centroid.begin();
          iterator_nodes != face_centroid.end(); ++iterator_nodes) {
        all_faces_centroids.push_back(*iterator_nodes);
      }
    }

    //Add the new faces to its corresponding segments and elements
    std::vector<Entity*> adjacent_segments;
    std::vector<Entity*> original_face_relations_3D;
    std::vector<Entity*> original_face;
    std::vector<Entity*>::iterator iterator_entities;
    for (unsigned int ii = 0; ii < all_boundary_segments.size(); ++ii) {
      adjacent_segments = find_adjacent_segments(*all_boundary_segments[ii],
          all_faces_centroids[ii / Num_segments_face]);
      add_relation(*(modified1_entities_2D[ii]), *(all_boundary_segments[ii]),
          0);
      add_relation(*(modified1_entities_2D[ii]), *(adjacent_segments[0]), 1);
      add_relation(*(modified1_entities_2D[ii]), *(adjacent_segments[1]), 2);

      //Add the new face to its corresponding element
      original_face = get_directly_connected_entities(
          *(all_faces_centroids[ii / Num_segments_face]), 2);

      //find original_face 3D relations (entities)
      original_face_relations_3D = find_3D_relations(*(original_face[0]));
      for (iterator_entities = original_face_relations_3D.begin();
          iterator_entities != original_face_relations_3D.end();
          iterator_entities++) {
        add_relation(*(*iterator_entities), *(modified1_entities_2D[ii]),
            get_number_lower_rank_entities(*(*iterator_entities)));
      }
    }

    //MEASURING TIME
        end4 = clock();
        cpu_time_used4 = ((double) (end4 - start4)) / CLOCKS_PER_SEC;
        std::cout << endl;
        std::cout << "The Fourth part takes "
             		  << cpu_time_used4 << " seconds"<<endl;

//-----------------------------------------------------------------------------------------------------------------------------------
// V. Delete former mesh faces
// initial_entities_3D:  Vector that contains all the former elements of the mesh
// All_boundary_faces:vector with all the boundary faces of all elements.
// This vector doesn't include the faces inside the elements
//-----------------------------------------------------------------------------------------------------------------------------------
      //MEASURING TIME
      clock_t start5, end5;
      double cpu_time_used5;
      start5 = clock();
    //Because "remove_entity" cannot be used to delete the relation between faces and elements
    //Remove first the relations between elements and faces
    std::vector<Entity*>::iterator iterator_entities_3D;
    std::vector<Entity*>::iterator iterator_faces_centroids;
    for (iterator_faces_centroids = all_faces_centroids.begin();
    	iterator_faces_centroids != all_faces_centroids.end();++iterator_faces_centroids){
    	std::vector<Entity*> former_face = get_directly_connected_entities(
    	          *(*iterator_faces_centroids), 2);
    	std::vector<Entity*> elements = get_directly_connected_entities(
    	          *(former_face[0]), 3);
    	for (iterator_entities_3D = elements.begin();
    	     iterator_entities_3D != elements.end(); ++iterator_entities_3D) {
    	     remove_relation(*(*iterator_entities_3D), *(former_face[0]),
    	     get_local_relation_id(*(*iterator_entities_3D), *(former_face[0])));
        }
    }

    //Remove the former mesh faces and all their relations
    std::vector<Entity*>::iterator iterator_all_faces_centroids;
    for (iterator_all_faces_centroids = all_faces_centroids.begin();
         iterator_all_faces_centroids != all_faces_centroids.end();
         ++iterator_all_faces_centroids){
    	std::vector<Entity*> former_face = get_directly_connected_entities(
    	*(*iterator_all_faces_centroids), 2);
    	remove_entity(*(former_face[0]));
    }


    //The following variables will be used in step IX
    //Number of faces per element "_faces_element". This number doesn't include any faces inside
    //the element.Only the ones at the boundary
    std::vector<Entity*> _faces_element;
    std::vector<Entity*> All_boundary_faces;
    std::vector<Entity*>::iterator iterator_faces_element;
    std::vector<Entity*>::iterator iterator_initial_entities_3D;
    for (iterator_initial_entities_3D = initial_entities_3D.begin();
    	iterator_initial_entities_3D != initial_entities_3D.end();++iterator_initial_entities_3D){
    	_faces_element = get_directly_connected_entities(
    	          *(*iterator_initial_entities_3D), 2);
    	for (iterator_faces_element = _faces_element.begin();
    	     iterator_faces_element != _faces_element.end();++iterator_faces_element) {
    	     All_boundary_faces.push_back(*iterator_faces_element);
    	}
    }
    //MEASURING TIME
    end5 = clock();
    cpu_time_used5 = ((double) (end5 - start5)) / CLOCKS_PER_SEC;
    std::cout << endl;
    std::cout << "The Fifth part takes "
            		  << cpu_time_used5 << " seconds"<<endl;
//-----------------------------------------------------------------------------------------------------------------------------------
// VI. Add a point to each element. Each point represents the centroid of each element
// modified2_entities_0D: Vector that contains all the nodes up to the centroids of all the elements
// Add the coordinates of all the elements centroids
//-----------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
    clock_t start6, end6;
    double cpu_time_used6;
    start6 = clock();
    //Add a point to each element
    std::vector<size_t> requests_step6(number_dimensions_ + 1, 0);
    requests_step6[0] = initial_entities_3D.size();
    add_entities(requests_step6);

    std::vector<Entity*> modified2_entities_0D = get_entities_by_rank(
        *(bulkData_), 0);

    //At this point the way the numbers are stored in the vector of nodes has changed
    //Thus, create a new vector with the nodes that has all the centroids of the elements
    std::vector<Entity*> elements_centroids;
    int _start = initial_entities_2D.size();
    int _end = initial_entities_2D.size() + initial_entities_3D.size();
    for (int ii = _start; ii < _end; ++ii) {
      elements_centroids.push_back(modified2_entities_0D[ii]);
    }

    //Add the coordinates of all the elements centroids
    std::vector<Entity*> boundary_nodes_elements;
    for (unsigned int ii = 0; ii < initial_entities_3D.size(); ++ii) {
      boundary_nodes_elements = get_former_element_nodes(
          *(initial_entities_3D[ii]), all_elements_boundary_nodes1);
      create_coordinates(boundary_nodes_elements, elements_centroids[ii]);
    }

    //Connect each element to the new added nodes
    for (unsigned int ii = 0; ii < initial_entities_3D.size(); ++ii) {
      //Connect the node to its corresponding element
      add_relation(*(initial_entities_3D[ii]), *(elements_centroids[ii]),
          get_number_lower_rank_entities(*(initial_entities_3D[ii])));
    }
    //MEASURING TIME
       end6 = clock();
       cpu_time_used6 = ((double) (end6 - start6)) / CLOCKS_PER_SEC;
       std::cout << endl;
       std::cout << "The Sixth part takes "
                      		  << cpu_time_used6 << " seconds"<<endl;

//-----------------------------------------------------------------------------------------------------------------------------------
// VII. For each element create new segments to connect its center point to all the points that
// compose its boundary
// modified3_entities_1D: Vector that contains all the segments up to the new ones defined in step VII.
//-----------------------------------------------------------------------------------------------------------------------------------

    //MEASURING TIME
    clock_t start7, end7;
    double cpu_time_used7;
    start7 = clock();
    //Add the new segments that will connect the center point with the points at the boundary
    //Create a vector with all the boundary points of all the former elements of the mesh
    std::vector<Entity*> element_boundary_nodes;
    std::vector<Entity*> all_elements_boundary_nodes;
    std::vector<Entity*>::iterator iterator_Initial_entities_3D;
    std::vector<Entity*>::iterator iterator_element_boundary_nodes;

    for (iterator_Initial_entities_3D = initial_entities_3D.begin();
    	iterator_Initial_entities_3D != initial_entities_3D.end(); ++iterator_Initial_entities_3D){
    	element_boundary_nodes = get_boundary_entities(*(*iterator_Initial_entities_3D),0);
    	for (iterator_element_boundary_nodes = element_boundary_nodes.begin();
    		 iterator_element_boundary_nodes != element_boundary_nodes.end();++iterator_element_boundary_nodes) {
    	     all_elements_boundary_nodes.push_back(*iterator_element_boundary_nodes);
    	}
    }

    //Add the new segments.
    std::vector<size_t> requests_step7(number_dimensions_ + 1, 0);
    requests_step7[1] = all_elements_boundary_nodes.size();
    add_entities(requests_step7);


    //Vector that contains the latest addition of segments
    std::vector<Entity*> modified3_entities_1D = get_entities_by_rank(
        *(bulkData_), 1);

    //At this point the way the numbers are stored in the vector of segments has changed
    //Thus, create a new vector that contains the segments that connect the element centroid
    //with the element boundary points
   const int Start_ = Num_segments_face * initial_entities_2D.size()
        + initial_entities_1D.size();
   const int End_ = modified3_entities_1D.size() - initial_entities_1D.size();
    std::vector<Entity*> segments_connected_centroid;
    for (int ii = Start_; ii < End_; ++ii) {
      segments_connected_centroid.push_back(modified3_entities_1D[ii]);
    }

    //Connect the new segments to the corresponding nodes
    for (unsigned int ii = 0; ii < segments_connected_centroid.size(); ++ii) {
      //Add a relation between the segments and the center nodes
      add_relation(*(segments_connected_centroid[ii]),
          *(elements_centroids[ii / element_boundary_nodes.size()]), 0);
      //Add a relation between the segments and the boundary nodes
      add_relation(*(segments_connected_centroid[ii]),
          *(all_elements_boundary_nodes[ii]), 1);
    }
    //MEASURING TIME
        end7 = clock();
        cpu_time_used7 = ((double) (end7 - start7)) / CLOCKS_PER_SEC;
        std::cout << endl;
        std::cout << "The Seventh part takes "
                    		  << cpu_time_used7 << " seconds"<<endl;

//-----------------------------------------------------------------------------------------------------------------------------------
// VIII. Create the new faces inside each element
// modified2_entities_2D: Vector with all the faces up the ones that are inside the elements
//-----------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
    clock_t start8, end8;
    double cpu_time_used8;
    start8 = clock();
    //Add the new faces.
    std::vector<size_t> requests_step8(number_dimensions_ + 1, 0);
    requests_step8[2] = All_boundary_segments.size();
    add_entities(requests_step8);

    std::vector<Entity*> modified2_entities_2D = get_entities_by_rank(
        *(bulkData_), 2);

    //Create a vector of vectors with all the faces inside the element. Each
    //row represents an element
    std::vector<std::vector<Entity*> > faces_inside_elements(initial_entities_3D.size());

    //Connect  the face to the corresponding segments
    std::vector<Entity*> adjacent_segments_inside(2);
    for (unsigned int ii = 0; ii < All_boundary_segments.size(); ++ii) {
        adjacent_segments_inside = find_adjacent_segments(*(All_boundary_segments[ii]),
        elements_centroids[ii / Number_new_triangles_inside_element]);
        add_relation(*(modified2_entities_2D[ii]), *(All_boundary_segments[ii]),
          0);
        add_relation(*(modified2_entities_2D[ii]), *(adjacent_segments_inside[0]),
          1);
        add_relation(*(modified2_entities_2D[ii]), *(adjacent_segments_inside[1]),
          2);
      /*
       *faces_inside_elements is a vector of vectors that contains
       *the faces inside each element. Each row contains the faces of
       *one specific element. The first row corresponds to the first
       *element, the second one to the second element, and so forth.
        */
      faces_inside_elements[ii/Number_new_triangles_inside_element].
      push_back(modified2_entities_2D[ii]);
    }

    //MEASURING TIME
       end8 = clock();
       cpu_time_used8 = ((double) (end8 - start8)) / CLOCKS_PER_SEC;
       std::cout << endl;
       std::cout << "The Eight part takes "
                 		  << cpu_time_used8 << " seconds"<<endl;

//-----------------------------------------------------------------------------------------------------------------------------------
// IX. Delete the former elements
//
//-----------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
      clock_t start9, end9;
      double cpu_time_used9;
      start9 = clock();
    //Remove former elements from the mesh
    std::vector<Entity*>::iterator iterator_elements_centroids;
    for (iterator_elements_centroids = elements_centroids.begin();
    		iterator_elements_centroids != elements_centroids.end();
    	++iterator_elements_centroids){
    	std::vector<Entity*> former_element = get_directly_connected_entities(
    	*(*iterator_elements_centroids), 3);
    	remove_entity(*(former_element[0]));
    }

    //MEASURING TIME
    end9 = clock();
    cpu_time_used9 = ((double) (end9 - start9)) / CLOCKS_PER_SEC;
    std::cout << endl;
    std::cout << "The Ninth part takes "
              		  << cpu_time_used9 << " seconds"<<endl;

//-----------------------------------------------------------------------------------------------------------------------------------
// X. Create the new elements
// modified1_entities_3D: Vector with all the elements required to carry out the barycentric
// subdivision
//-----------------------------------------------------------------------------------------------------------------------------------

   const int number_new_elements = _faces_element.size()*initial_entities_3D.size();

    //Add the new elements
    for (int ii = 0; ii < number_new_elements; ++ii) {
      add_element(3);
    }
    std::vector<Entity*> modified1_entities_3D = get_entities_by_rank(*(bulkData_),3);

    //MEASURING TIME
      clock_t start10, end10;
      double cpu_time_used10;
      start10 = clock();
    //Connect the the element with its corresponding faces
    std::vector<Entity*> adjacent_faces_inside(3);
    for (unsigned int ii = 0; ii < All_boundary_faces.size(); ++ii) {
         adjacent_faces_inside  = find_adjacent_faces_from_face(faces_inside_elements,
         *(All_boundary_faces[ii]),(ii/_faces_element.size()));
      add_relation(*(modified1_entities_3D[ii]), *(All_boundary_faces[ii]), 0);
      add_relation(*(modified1_entities_3D[ii]), *adjacent_faces_inside[0], 1);
      add_relation(*(modified1_entities_3D[ii]), *adjacent_faces_inside[1], 2);
      add_relation(*(modified1_entities_3D[ii]), *adjacent_faces_inside[2], 3);
    }

    //MEASURING TIME
       end10 = clock();
       cpu_time_used10 = ((double) (end10 - start10)) / CLOCKS_PER_SEC;
       std::cout << endl;
       std::cout << "The tenth part takes "
                		  << cpu_time_used10 << "seconds"<<endl;


//-----------------------------------------------------------------------------------------------------------------------------------
// XI. Update the vector: connectivity_temp
//-----------------------------------------------------------------------------------------------------------------------------------
    //MEASURING TIME
    clock_t start11, end11;
    double cpu_time_used11;
    start11 = clock();
    //Connectivity matrix
    std::vector<std::vector<Entity*> > _connectivity_temp_(modified1_entities_3D.size());
    //Add the new entities to "connectivity_temp"
    for (unsigned int ii = 0; ii < modified1_entities_3D.size(); ++ii) {
         _connectivity_temp_[ii] = get_boundary_entities(*modified1_entities_3D[ii],0);
    }
    connectivity_temp.clear();
    connectivity_temp = _connectivity_temp_;

    // End mesh update
    bulkData_->modification_end();
    //MEASURING TIME
       end11 = clock();
       cpu_time_used11 = ((double) (end11 - start11)) / CLOCKS_PER_SEC;
       std::cout << endl;
       std::cout << "The Eleventh part takes "
                    		  << cpu_time_used11 << " seconds"<<endl;

    return;
  }

} // namespace LCM

#endif // #if defined (ALBANY_LCM)
