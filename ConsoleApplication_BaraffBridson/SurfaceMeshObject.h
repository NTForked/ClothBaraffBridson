#ifndef CLOTH_MODEL
#define CLOTH_MODEL

#include "Model.h"
#include "BasicTypes.h"

#include <assimp\types.h>
#include <map>
#include <functional>

//#define OPENMESH_BASED
#define CGAL_BASED

#ifdef OPENMESH_BASED

#include <OpenMesh/Core/Mesh/TriMesh_ArrayKernelT.hh>
#include <OpenMesh/Core/Mesh/PolyMesh_ArrayKernelT.hh>

//typedef OpenMesh::TriMesh_ArrayKernelT<> TriArrayMesh;
//typedef OpenMesh::PolyMesh_ArrayKernelT<> PolyArrayMesh;

class SurfaceMeshObject
{
public:
	SurfaceMeshObject(GLuint edges) :
		EDGES(edges) {}

	PolyArrayMesh* getMesh()
	{
		return PolyMesh;
	}

	void import(const Mesh mesh);
	
	void exportPos3fNorm3fBuffer(
		GLfloat* & vertexBuffer, GLfloat* & vertexNormalBuffer, GLuint & vertexSize, 
		GLuint* & elementBuffer, GLuint & elementSize);

	void exportFaceNorm3fBuffer(
		GLfloat* & fBarycenterBuffer, GLfloat* & fNormalBuffer, GLuint & faceSize);

	bool useVTexCoord2DAsVPlanarCoord3f();

	bool getVPlanarCoord3f(OpenMesh::VPropHandleT<OpenMesh::Vec3f> & vph);

private:
	const GLuint EDGES;
	PolyArrayMesh* PolyMesh = new PolyArrayMesh();

	//template <typename PropType>
	//OpenMesh::VPropHandleT<PropType> SurfaceMeshObject::addVProp(std::string propName, PropType value);

	template <typename PropType>
	OpenMesh::VPropHandleT<PropType> addVProp(
		std::string propName,
		std::function<PropType(PolyArrayMesh::VertexHandle)> vhandle2value
		);

};
#endif

#ifdef CGAL_BASED

#include <CGAL/Simple_cartesian.h>
#include <CGAL/Surface_mesh.h>
#include <CGAL/Polygon_mesh_processing/compute_normal.h>
#include <CGAL\Polyhedron_3.h>
#include <CGAL/Exact_predicates_inexact_constructions_kernel.h>

#include <boost/foreach.hpp>

#include <Eigen\Core>

#include <map>

//// kernel type
//typedef CGAL::Simple_cartesian<float> Kernelf;
//// primitive types
//typedef Kernelf::FT FT;
//typedef Kernelf::Point_3 Point3f;
//typedef Kernelf::Vector_3 Vec3f;
//typedef Kernelf::Triangle_3 Triangle3f;
//typedef Kernelf::Segment_3 Segment3f;
//// surface mesh type
//typedef CGAL::Surface_mesh<Point3f> SurfaceMesh3f;
//// primitive index types
//typedef SurfaceMesh3f::Vertex_index Veridx;
//typedef SurfaceMesh3f::Face_index Faceidx;
//typedef SurfaceMesh3f::Edge_index Edgeidx;
//typedef SurfaceMesh3f::Halfedge_index Halfedgeidx;

class SurfaceMeshObject
{
public:
	SurfaceMeshObject(GLuint edges) :
		EDGES(edges), PolyMesh(new SurfaceMesh3f())
	{}

	SurfaceMesh3f* getMesh()
	{
		return PolyMesh;
	}

	void import(const Mesh mesh);

	bool getVPlanarCoord3f(SurfaceMesh3f::Property_map<Veridx, Point3f> & vph);

	const std::string pname_texCoords = "v:texture_coordinates";
	const std::string pname_vertexPlanarCoords = "v:vertex_planar_coordinates";
	const std::string pname_vertexNormals = "v:vertex_normals";
	const std::string pname_vertexLastPositions = "v:vertex_last_positions";
	const std::string pname_vertexPredictPositions = "v:vertex_predict_positions";
	const std::string pname_faceNormals = "v:face_normals";

	/* -------- exporters for drawing ---------- */
	void exportPos3fNorm3fBuffer(
		GLfloat* & vertexBuffer, GLfloat* & vertexNormalBuffer, GLuint & vertexSize,
		GLuint* & elementBuffer, GLuint & elementSize) const;

	void exportFaceNorm3fBuffer(
		GLfloat* & fBarycenterBuffer, GLfloat* & fNormalBuffer, GLuint & faceSize) const;

	/* -------- set vertices' properties --------- */
	bool useVTexCoord2DAsVPlanarCoord3f();

	void addPositionsProperty()
	{
		SurfaceMesh3f::Property_map<Veridx, Point3f> vprop_handle =
			PolyMesh->add_property_map<Veridx, Point3f>(pname_vertexLastPositions).first;
		for (Veridx vhd : PolyMesh->vertices())
			vprop_handle[vhd] = PolyMesh->point(vhd);
		vprop_handle =
			PolyMesh->add_property_map<Veridx, Point3f>(pname_vertexPredictPositions).first;
		for (Veridx vhd : PolyMesh->vertices())
			vprop_handle[vhd] = PolyMesh->point(vhd);
	}

	/* -------- getters and setters --------- */
	Eigen::VectorXf getPositions() const;
	void setPositions(Eigen::VectorXf const & positions);

	size_t getVertexSize() { return VERTEX_SIZE; }
	size_t getFaceSize() { return FACE_SIZE; }
	size_t getEdgeSize() { return EDGE_SIZE; }
	
	std::map<Veridx, GLuint> const * getVertices2indices() { return vertices2indices; }
	std::map<Faceidx, GLuint> const * getFaces2indices() { return faces2indices; }
	std::map<Edgeidx, GLuint> const * getEdges2indices() { return edges2indices; }

	void RecomputeNormals();
	void Affine(const Eigen::Matrix4f& matrix);

private:
	const GLuint EDGES;

	SurfaceMesh3f* PolyMesh;

	/* mesh properties hash tables */
	std::map<Veridx, GLuint> * vertices2indices;
	std::map<Faceidx, GLuint> * faces2indices;
	std::map<Edgeidx, GLuint> * edges2indices;

	GLuint VERTEX_SIZE, FACE_SIZE, EDGE_SIZE;

};

#endif

#endif


