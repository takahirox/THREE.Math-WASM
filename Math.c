#include <stdlib.h>
//#define CACHED

struct LinkedList {
	struct LinkedList *next;
	struct LinkedList *prev;
	void *value;
};

struct LinkedListHeader {
	struct LinkedList *head;
	struct LinkedList *tail;
};

struct Vector3 {
	double x;
	double y;
	double z;
};

struct Quaternion {
	double x;
	double y;
	double z;
	double w;
};

struct Matrix4 {
	double elements[16];
};

struct Object3D {
	struct Vector3 position;
	struct Quaternion quaternion;
	struct Vector3 scale;
	struct Matrix4 matrix;
	struct Matrix4 matrixWorld;
	int matrixAutoUpdate;
	int matrixWorldNeedsUpdate;
	struct Object3D *parent;
	int childrenNum;
	struct LinkedListHeader children;
#ifdef CACHED
	struct Vector3 cachedPosition;
	struct Quaternion cachedQuaternion;
	struct Vector3 cachedScale;
#endif
};

struct LinkedList* LinkedList_new(
	void *value
) {
	struct LinkedList *p = malloc(sizeof(struct LinkedList));
	p->next = 0;
	p->prev = 0;
	p->value = value;
	return p;
};

struct LinkedListHeader* LinkedListHeader_init(
	struct LinkedListHeader *self
) {
	self->head = 0;
	self->tail = 0;
	return self;
};

struct LinkedListHeader* LinkedListHeader_add(
	struct LinkedListHeader *self,
	void *value
) {
	struct LinkedList *node = LinkedList_new(value);

	if (self->tail == 0) {
		self->head = node;
		self->tail = node;
	} else {
		node->prev = self->tail;
		self->tail->next = node;
		self->tail = node;
	}

	return self;
};

struct Vector3* Vector3_init(
	struct Vector3 *self
) {
	self->x = 0;
	self->y = 0;
	self->z = 0;
	return self;
}

struct Vector3* Vector3_copy(
	struct Vector3 *self,
	struct Vector3 *v
) {
	self->x = v->x;
	self->y = v->y;
	self->z = v->z;
	return self;
}

int Vector3_equals(
	struct Vector3 *self,
	struct Vector3 *v
) {
	return self->x == v->x &&
		self->y == v->y &&
		self->z == v->z;
}

struct Quaternion* Quaternion_init(
	struct Quaternion *self
) {
	self->x = 0;
	self->y = 0;
	self->z = 0;
	self->w = 1;
	return self;
}

int Quaternion_equals(
	struct Quaternion *self,
	struct Quaternion *q
) {
	return self->x == q->x &&
		self->y == q->y &&
		self->z == q->z &&
		self->w == q->w;
}

struct Quaternion* Quaternion_copy(
	struct Quaternion *self,
	struct Quaternion *q
) {
	self->x = q->x;
	self->y = q->y;
	self->z = q->z;
	self->w = q->w;
	return self;
}

struct Matrix4* Matrix4_init(
	struct Matrix4 *self
) {
	self->elements[0] = 1.0;
	self->elements[1] = 0.0;
	self->elements[2] = 0.0;
	self->elements[3] = 0.0;
	self->elements[4] = 0.0;
	self->elements[5] = 1.0;
	self->elements[6] = 0.0;
	self->elements[7] = 0.0;
	self->elements[8] = 0.0;
	self->elements[9] = 0.0;
	self->elements[10] = 1.0;
	self->elements[11] = 0.0;
	self->elements[12] = 0.0;
	self->elements[13] = 0.0;
	self->elements[14] = 0.0;
	self->elements[15] = 1.0;
	return self;
}

struct Matrix4* Matrix4_compose(
	struct Matrix4 *self,
	struct Vector3 *position,
	struct Quaternion *quaternion,
	struct Vector3 *scale
) {
	double x = quaternion->x;
	double y = quaternion->y;
	double z = quaternion->z;
	double w = quaternion->w;

	double x2 = x + x;
	double y2 = y + y;
	double z2 = z + z;

	double xx = x * x2;
	double xy = x * y2;
	double xz = x * z2;

	double yy = y * y2;
	double yz = y * z2;
	double zz = z * z2;

	double wx = w * x2;
	double wy = w * y2;
	double wz = w * z2;

	double sx = scale->x;
	double sy = scale->y;
	double sz = scale->z;

	self->elements[0] = (1.0 - (yy + zz)) * sx;
	self->elements[1] = (xy + wz) * sx;
	self->elements[2] = (xz - wy) * sx;
	self->elements[3] = 0.0;

	self->elements[4] = (xy - wz) * sy;
        self->elements[5] = (1.0 - (xx + zz)) * sy;
        self->elements[6] = (yz + wx) * sy;
        self->elements[7] = 0.0;

        self->elements[8] = (xz + wy) * sz;
        self->elements[9] = (yz - wx) * sz;
        self->elements[10] = (1.0 - (xx + yy)) * sz;
        self->elements[11] = 0.0;

        self->elements[12] = position->x;
        self->elements[13] = position->y;
        self->elements[14] = position->z;
        self->elements[15] = 1.0;

	return self;
}

struct Matrix4* Matrix4_copy(
	struct Matrix4 *self,
	struct Matrix4 *m
) {
	self->elements[0] = m->elements[0];
	self->elements[1] = m->elements[1];
	self->elements[2] = m->elements[2];
	self->elements[3] = m->elements[3];
	self->elements[4] = m->elements[4];
	self->elements[5] = m->elements[5];
	self->elements[6] = m->elements[6];
	self->elements[7] = m->elements[7];
	self->elements[8] = m->elements[8];
	self->elements[9] = m->elements[9];
	self->elements[10] = m->elements[10];
	self->elements[11] = m->elements[11];
	self->elements[12] = m->elements[12];
	self->elements[13] = m->elements[13];
	self->elements[14] = m->elements[14];
	self->elements[15] = m->elements[15];

	return self;
}

struct Matrix4* Matrix4_multiplyMatrices(
	struct Matrix4 *self,
	struct Matrix4 *a,
	struct Matrix4 *b
) {
	double a11 = a->elements[0];
	double a12 = a->elements[4];
	double a13 = a->elements[8];
	double a14 = a->elements[12];
	double a21 = a->elements[1];
	double a22 = a->elements[5];
	double a23 = a->elements[9];
	double a24 = a->elements[13];
	double a31 = a->elements[2];
	double a32 = a->elements[6];
	double a33 = a->elements[10];
	double a34 = a->elements[14];
	double a41 = a->elements[3];
	double a42 = a->elements[7];
	double a43 = a->elements[11];
	double a44 = a->elements[15];

	double b11 = b->elements[0];
	double b12 = b->elements[4];
	double b13 = b->elements[8];
	double b14 = b->elements[12];
	double b21 = b->elements[1];
	double b22 = b->elements[5];
	double b23 = b->elements[9];
	double b24 = b->elements[13];
	double b31 = b->elements[2];
	double b32 = b->elements[6];
	double b33 = b->elements[10];
	double b34 = b->elements[14];
	double b41 = b->elements[3];
	double b42 = b->elements[7];
	double b43 = b->elements[11];
	double b44 = b->elements[15];

	self->elements[0] = a11 * b11 + a12 * b21 + a13 * b31 + a14 * b41;
	self->elements[4] = a11 * b12 + a12 * b22 + a13 * b32 + a14 * b42;
	self->elements[8] = a11 * b13 + a12 * b23 + a13 * b33 + a14 * b43;
	self->elements[12] = a11 * b14 + a12 * b24 + a13 * b34 + a14 * b44;

	self->elements[1] = a21 * b11 + a22 * b21 + a23 * b31 + a24 * b41;
	self->elements[5] = a21 * b12 + a22 * b22 + a23 * b32 + a24 * b42;
	self->elements[9] = a21 * b13 + a22 * b23 + a23 * b33 + a24 * b43;
	self->elements[13] = a21 * b14 + a22 * b24 + a23 * b34 + a24 * b44;

	self->elements[2] = a31 * b11 + a32 * b21 + a33 * b31 + a34 * b41;
	self->elements[6] = a31 * b12 + a32 * b22 + a33 * b32 + a34 * b42;
	self->elements[10] = a31 * b13 + a32 * b23 + a33 * b33 + a34 * b43;
	self->elements[14] = a31 * b14 + a32 * b24 + a33 * b34 + a34 * b44;

	self->elements[3] = a41 * b11 + a42 * b21 + a43 * b31 + a44 * b41;
	self->elements[7] = a41 * b12 + a42 * b22 + a43 * b32 + a44 * b42;
	self->elements[11] = a41 * b13 + a42 * b23 + a43 * b33 + a44 * b43;
	self->elements[15] = a41 * b14 + a42 * b24 + a43 * b34 + a44 * b44;

	return self;
}

struct Object3D* Object3D_init(
	struct Object3D *self
) {
	Vector3_init(&self->position);
	Quaternion_init(&self->quaternion);
	Vector3_init(&self->scale);
	self->scale.x = 1;
	self->scale.y = 1;
	self->scale.z = 1;
#ifdef CACHED
	Vector3_copy(&self->cachedScale, &self->scale);
#endif
	Matrix4_init(&self->matrix);
	Matrix4_init(&self->matrixWorld);
	self->matrixAutoUpdate = 1;
	self->matrixWorldNeedsUpdate = 0;
	self->parent = 0;
	self->childrenNum = 0;
	LinkedListHeader_init(&self->children);
	return self;
}

struct Object3D* Object3D_add(
	struct Object3D *self,
	struct Object3D *child
) {
	LinkedListHeader_add(&self->children, child);
	self->childrenNum++;
	child->parent = self;
	return self;
}

void Object3D_updateMatrix(
	struct Object3D *self
) {
#ifdef CACHED
	if (! Vector3_equals(&self->position, &self->cachedPosition) ||
		! Quaternion_equals(&self->quaternion, &self->cachedQuaternion) ||
		! Vector3_equals(&self->scale, &self->cachedScale)) {
#endif
		Matrix4_compose(
			&self->matrix,
			&self->position,
			&self->quaternion,
			&self->scale
		);

		self->matrixWorldNeedsUpdate = 1;
#ifdef CACHED
		Vector3_copy(&self->cachedPosition, &self->position);
		Quaternion_copy(&self->cachedQuaternion, &self->quaternion);
		Vector3_copy(&self->cachedScale, &self->scale);
	}
#endif
}

void Object3D_updateMatrixWorld(
	struct Object3D *self,
	int force
) {
	if (self->matrixAutoUpdate == 1) Object3D_updateMatrix(self);

	if (self->matrixWorldNeedsUpdate == 1 || force == 1) {
		if (self->parent == 0) {
			Matrix4_copy(&self->matrixWorld, &self->matrix);
		} else {
			Matrix4_multiplyMatrices(
				&self->matrixWorld,
				&self->parent->matrixWorld,
				&self->matrix
			);
		}

		self->matrixWorldNeedsUpdate = 0;
		force = 1;
	}

	int childrenNum = self->childrenNum;

	struct LinkedList *node = self->children.head;

	while (node != 0) {
		Object3D_updateMatrixWorld(node->value, force);
		node = node->next;
	}
}

int getObjectSize() {
	return sizeof(struct Object3D);
}
