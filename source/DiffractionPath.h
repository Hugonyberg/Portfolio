#pragma once
#include <DreamEngine\math\Vector.h>

struct GO_Matrix
{
	float m0, m4, m8, m12;  // Matrix first row (4 components)
	float m1, m5, m9, m13;  // Matrix second row (4 components)
	float m2, m6, m10, m14; // Matrix third row (4 components)
	float m3, m7, m11, m15; // Matrix fourth row (4 components)
};
struct Own_Transform
{
	DE::Vector3f position{ 0,0,0 };
	DE::Vector3f right{ 1,0,0 };
	DE::Vector3f up{ 0,1,0 };
	DE::Vector3f forward{ 0,0,1 };
	DE::Vector3f scale{ 1,1,1 };
	GO_Matrix matrix;
};

struct DiffractionPath
{
	/// Defines the maximum number of nodes that a user can retrieve information about.  Longer paths will be truncated. 
	static const int kMaxNodes = 4;

	/// Diffraction points along the path. nodes[0] is the point closest to the listener; nodes[numNodes-1] is the point closest to the emitter. 
	/// Neither the emitter position nor the listener position are represented in this array.
	DE::Vector3f nodes[kMaxNodes];

	/// Emitter position. This is the source position for an emitter. In all cases, except for radial emitters, it is the same position as the game object position.
	/// For radial emitters, it is the calculated position at the edge of the volume.
	DE::Vector3f emitterPos;

	/// Raw diffraction angles at each point, in radians.
	float angles[kMaxNodes];

	/// ID of the portals that the path passes through.  For a given node at position i (in the nodes array), if the path diffracts on a geometric edge, then portals[i] will be an invalid portal ID (ie. portals[i].IsValid() will return false). 
	/// Otherwise, if the path diffracts through a portal at position i, then portals[i] will be the ID of that portal.
	/// portal[0] represents the node closest to the listener; portal[numNodes-1] represents the node closest to the emitter.
	int portals[kMaxNodes];

	/// ID's of the rooms that the path passes through. For a given node at position i, room[i] is the room on the listener's side of the node. If node i diffracts through a portal, 
	/// then rooms[i] is on the listener's side of the portal, and rooms[i+1] is on the emitters side of the portal.
	/// There is always one extra slot for a room so that the emitters room is always returned in slot room[numNodes] (assuming the path has not been truncated).
	int rooms[kMaxNodes + 1];

	/// Virtual emitter position. This is the position that is passed to the sound engine to render the audio using multi-positioning, for this particular path.
	Own_Transform virtualPos;

	/// Total number of nodes in the path.  Defines the number of valid entries in the \c nodes, \c angles, and \c portals arrays. The \c rooms array has one extra slot to fit the emitter's room.
	int nodeCount;

	/// Calculated total diffraction from this path, normalized to the range [0,1]
	/// The diffraction amount is calculated from the sum of the deviation angles from a straight line, of all angles at each nodePoint. 
	//	Can be thought of as how far into the 'shadow region' the sound has to 'bend' to reach the listener.
	/// This value is applied internally, by spatial audio, as the Diffraction value and built-in parameter of the emitter game object.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	float diffraction;

	/// Calculated total transmission loss from this path, normalized to the range [0,1]
	/// This field will be 0 for diffraction paths where \c nodeCount > 0. It may be non-zero for the direct path where \c nodeCount == 0.
	/// The path's transmission loss value is the combination of the geometric transmission loss and the room transmission loss, by taking the greater of the two.
	/// The geometric transmission loss is calculated from the transmission loss values assigned to the geometry that this path transmits through.
	/// If a path transmits through multiple geometries with different transmission loss values, the largest value is taken.
	/// The room transmission loss is taken from the emitter and listener rooms' transmission loss values, and likewise, 
	/// if the listener's room and the emitter's room have different transmission loss values, the greater of the two is used.
	/// This value is applied internally, by spatial audio, as the Transmission Loss value and built-in parameter of the emitter game object.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	/// - \ref AkRoomParams
	/// - \ref AkAcousticSurface
	float transmissionLoss;

	/// Total path length
	/// Represents the sum of the length of the individual segments between nodes, with a correction factor applied for diffraction. 
	/// The correction factor simulates the phenomenon where by diffracted sound waves decay faster than incident sound waves and can be customized in the spatial audio init settings.
	/// \sa
	/// - \ref AkSpatialAudioInitSettings
	float totLength;

	/// Obstruction value for this path 
	/// This value includes the accumulated portal obstruction for all portals along the path.
	float obstructionValue;
};

