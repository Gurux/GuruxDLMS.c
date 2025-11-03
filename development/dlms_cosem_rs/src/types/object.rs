use std::vec::Vec;

#[cfg(feature = "serde")]
use serde::{Deserialize, Serialize};

/// Collection wrapper mimicking the semantics of the legacy `objectArray` helper.
#[derive(Clone, Debug, Default, PartialEq, Eq)]
#[cfg_attr(feature = "serde", derive(Serialize, Deserialize))]
pub struct ObjectArray<T> {
    items: Vec<T>,
}

impl<T> ObjectArray<T> {
    /// Creates an empty container.
    pub fn new() -> Self {
        Self { items: Vec::new() }
    }

    /// Creates a container with the specified capacity.
    pub fn with_capacity(capacity: usize) -> Self {
        Self {
            items: Vec::with_capacity(capacity),
        }
    }

    /// Creates a container from an existing set of objects.
    pub fn from_vec(items: Vec<T>) -> Self {
        Self { items }
    }

    /// Number of objects stored in the container.
    pub fn len(&self) -> usize {
        self.items.len()
    }

    /// Returns `true` when no objects are stored.
    pub fn is_empty(&self) -> bool {
        self.items.is_empty()
    }

    /// Adds a new object to the container.
    pub fn push(&mut self, item: T) {
        self.items.push(item);
    }

    /// Extends the container with the provided objects.
    pub fn extend<I>(&mut self, iter: I)
    where
        I: IntoIterator<Item = T>,
    {
        self.items.extend(iter);
    }

    /// Provides read-only access to the items.
    pub fn iter(&self) -> core::slice::Iter<'_, T> {
        self.items.iter()
    }

    /// Provides mutable access to the items.
    pub fn iter_mut(&mut self) -> core::slice::IterMut<'_, T> {
        self.items.iter_mut()
    }

    /// Removes all objects while retaining the allocated capacity.
    pub fn clear(&mut self) {
        self.items.clear();
    }

    /// Consumes the container returning the inner vector.
    pub fn into_vec(self) -> Vec<T> {
        self.items
    }

    /// Returns a shared slice view of the items.
    pub fn as_slice(&self) -> &[T] {
        &self.items
    }

    /// Returns a mutable slice view of the items.
    pub fn as_mut_slice(&mut self) -> &mut [T] {
        &mut self.items
    }
}

impl<T> IntoIterator for ObjectArray<T> {
    type Item = T;
    type IntoIter = std::vec::IntoIter<T>;

    fn into_iter(self) -> Self::IntoIter {
        self.items.into_iter()
    }
}

impl<'a, T> IntoIterator for &'a ObjectArray<T> {
    type Item = &'a T;
    type IntoIter = core::slice::Iter<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter()
    }
}

impl<'a, T> IntoIterator for &'a mut ObjectArray<T> {
    type Item = &'a mut T;
    type IntoIter = core::slice::IterMut<'a, T>;

    fn into_iter(self) -> Self::IntoIter {
        self.iter_mut()
    }
}

/// Convenience alias emphasising the usage of DLMS/COSEM objects.
pub type ObjectList<T> = ObjectArray<T>;
