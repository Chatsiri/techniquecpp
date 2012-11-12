/*                       Titles                                          Authors                        Date
 *Create DFS and TypeTireries                                            Chatsiri.rat                   06/10/2012
 *Separate trie class and dfs																		         Chatsiri.rat
*/

#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/tuple/tuple.hpp>
#include <boost/tuple/tuple_io.hpp>

#include "gtest/gtest.h"

#include <iostream>
#include <string>
#include <list>
#include <deque>

#include <logger/clutil_logger.hpp>
#include <config/options_system.hpp>

using namespace boost;

namespace util = hnmav_util;

template<typename TypeTire, typename TypeNode>
struct node_tire {
    // data in node
    TypeNode const *data;

    int addr_node;

    // address insert next  node
    std::vector<node_tire<TypeTire, TypeNode> > addr_next_node;

    node_tire() : data(NULL) { }
};


template<typename TypeTire, typename TypeNode>
class tire
{
    public:
        tire( );
        node_tire<TypeTire, TypeNode> *check_root_node(node_tire<TypeTire, TypeNode>&   node);
        bool add_node(TypeTire const& data);
        node_tire<TypeTire, TypeNode> *recursive_node(node_tire<TypeTire, TypeNode>& node_branch, node_tire<TypeTire, TypeNode>& node_data);

    private:

        node_tire<TypeTire, TypeNode>   *node_data;

        std::vector<node_tire<TypeTire, TypeNode>* > node_root_vec;

        tuple<node_tire<TypeTire, TypeNode>, node_tire<TypeTire, TypeNode> > *node_compare;
        // handling node
        std::vector<shared_ptr<node_tire<TypeTire, TypeNode> > > node_root_shared_ptr;
        typedef shared_ptr<node_tire<TypeTire, TypeNode> > node_ptr;

        shared_ptr<util::clutil_logging<std::string, int> > *logger_ptr;
        util::clutil_logging<std::string, int> *logger;

};

template<typename TypeTire, typename TypeNode>
tire<TypeTire, TypeNode>::tire()
{

    util::options_system& op_system = util::options_system::get_instance();

    //init logger
    logger_ptr = &util::clutil_logging<std::string, int>::get_instance();
    logger = logger_ptr->get();
    std::string logger_main_file = op_system.get_logger_mainfile_path();
    std::string logger_settings  = op_system.get_logger_settingsfile_path();

    logger->add_logger(logger_main_file);
    logger->add_settings(logger_settings);
    logger->init_backend();
    logger->formatter_normal();
    logger->init_frontend();

}

// Finding data in root node before travelling in branch.
template<typename TypeTire, typename TypeNode>
node_tire<TypeTire, TypeNode> *tire<TypeTire, TypeNode>::check_root_node(node_tire<TypeTire, TypeNode>& node)
{
    int index_root_node = 0;
    node_tire<TypeTire, TypeNode> *root_vertex = NULL;

    // start node with null
    if( node_root_vec.size() == 0)
        return root_vertex;

    for(typename std::vector<node_tire<TypeTire, TypeNode>* >::iterator iter_root = node_root_vec.begin();
            iter_root != node_root_vec.end();
            ++iter_root, ++index_root_node) {
        node_tire<TypeTire, TypeNode> *node_in_vec = *iter_root;

        TypeNode const *data = node_in_vec->data;

        if( *data == *node.data) {
            node_tire<TypeTire, TypeNode>   *node_in_root = node_root_vec[index_root_node];
            root_vertex = node_root_vec[index_root_node];
        }
    }

    return root_vertex;
}

template<typename TypeTire, typename TypeNode>
bool tire<TypeTire, TypeNode>::add_node(TypeTire const& input_data)
{
    node_data  = new node_tire<TypeTire, TypeNode>();
    node_tire<TypeTire, TypeNode> *vertex_root = NULL;
    node_tire<TypeTire, TypeNode> *node_temp = NULL;

    for(typename TypeTire::const_iterator iter_input = input_data.begin(); iter_input != input_data.end(); ++iter_input) {

        if(node_data->data == NULL) {
            node_data->data = &(*iter_input);

            vertex_root = check_root_node(*node_data);

            if(vertex_root == NULL) {
                node_root_vec.push_back(node_data);

            } else {
                node_temp = node_data;
            }

            continue;
        }

        // Child node creates in addr_next_node
        node_tire<TypeTire, TypeNode> *child_node = new node_tire<TypeTire, TypeNode>();
        child_node->data = &(*iter_input);

        node_data->addr_next_node.push_back(*child_node);
        // next node
        node_data = &node_data->addr_next_node.back();
    }

    if(vertex_root  != NULL) {
        // Next from root vertex
        node_temp = &node_temp->addr_next_node.back();
        vertex_root = &vertex_root->addr_next_node.back();

        vertex_root = recursive_node(*vertex_root, *node_temp);

    }

    return true;
}
/*                      [root_node1...]
 *                  [A root]<- Node root checked by for loops           [ B root]
 *                /                            													/
 *               [B]                                                  [B]
 *               /                                                     /
 *             [B] <--Check Branch                                    [B]
 *           /    \                                                   /
 *        [C]      [D]                                              [E]
 *       /   \         \
 *    [E]     [F]      [E]
 *
 *    ABCE, ABDE
 */

template<typename TypeTire, typename TypeNode>
node_tire<TypeTire, TypeNode> *tire<TypeTire, TypeNode>::recursive_node(node_tire<TypeTire, TypeNode>& vertex_root, node_tire<TypeTire, TypeNode>& node_data)
{
    logger->write_info("   ----Start recursive  ");
    logger->write_info("   ---- Start recursive node with data input ", node_data.data);
    logger->write_info("   ---- Start recursive node with Vertex input ", vertex_root.data);
    logger->write_info("   ---- Vertex root size ", std::to_string(vertex_root.addr_next_node.size()) );

    std::deque<node_tire<TypeTire, TypeNode> > deque_node;
    node_tire<TypeTire, TypeNode> temp_vertex_root = vertex_root;

  while(&vertex_root != NULL) {
        // Vertex
        if(*(vertex_root.data) == *(node_data.data)) {

            deque_node.push_back(vertex_root);

            node_data = node_data.addr_next_node.back();
						vertex_root = vertex_root.addr_next_node.back();
        }


        // compare in branch of vertex
        for(typename std::vector<node_tire<TypeTire, TypeNode> >::iterator iter_node = vertex_root.addr_next_node.begin();
                iter_node != vertex_root.addr_next_node.end();
                ++iter_node ) {

            node_tire<TypeTire, TypeNode>& node_branch = *iter_node;

            if(*(node_branch.data) == *(node_data.data)) {

                deque_node.push_back(node_branch);
                node_data = node_data.addr_next_node.back();

                logger->write_info("--- Node Branch Data ", node_branch.data);
                logger->write_info("--- Node Data ", node_data.data);

            }

        }

        std::vector< node_tire<TypeTire, TypeNode> > vec_in_node = vertex_root.addr_next_node;

        if(vec_in_node.size() == 0) {
            break;
        } else {
            vertex_root = vec_in_node.back();
        }
    }

    logger->write_info("--- Deque ---");

    if(deque_node.size() != 0) {
        node_tire<TypeTire, TypeNode>  node_last = deque_node.back();

        node_last.addr_next_node.push_back(node_data);

        logger->write_info("*** Node last contain data ", node_last.data);
				logger->write_info("*** Node last contain data in size ", std::to_string(node_last.addr_next_node.size()));
        logger->write_info("*** Node data add to tail ", node_data.data);

        deque_node.clear();
    }

    vertex_root = temp_vertex_root;
    logger->write_info("###----End of recursive add node----### ");

    return &vertex_root;

}
