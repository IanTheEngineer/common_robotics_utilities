#pragma once

#include <cmath>
#include <cstdint>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include <Eigen/Geometry>
#include <common_robotics_utilities/maybe.hpp>
#include <common_robotics_utilities/serialization.hpp>
#include <common_robotics_utilities/utility.hpp>

namespace common_robotics_utilities
{
namespace voxel_grid
{
class GridIndex
{
private:
  int64_t x_ = -1;
  int64_t y_ = -1;
  int64_t z_ = -1;

public:
  GridIndex() : x_(-1), y_(-1), z_(-1) {}

  GridIndex(const int64_t x, const int64_t y, const int64_t z)
      : x_(x), y_(y), z_(z) {}

  const int64_t& X() const { return x_; }

  const int64_t& Y() const { return y_; }

  const int64_t& Z() const { return z_; }

  int64_t& X() { return x_; }

  int64_t& Y() { return y_; }

  int64_t& Z() { return z_; }

  bool operator==(const GridIndex& other) const
  {
    return (X() == other.X() && Y() == other.Y() && Z() == other.Z());
  }

  bool operator!=(const GridIndex& other) const
  {
    return !(*this == other);
  }
};

class GridSizes
{
private:
  double cell_x_size_ = 0.0;
  double cell_y_size_ = 0.0;
  double cell_z_size_ = 0.0;
  double inv_cell_x_size_ = 0.0;
  double inv_cell_y_size_ = 0.0;
  double inv_cell_z_size_ = 0.0;
  double x_size_ = 0.0;
  double y_size_ = 0.0;
  double z_size_ = 0.0;
  int64_t num_x_cells_ = 0;
  int64_t num_y_cells_ = 0;
  int64_t num_z_cells_ = 0;
  int64_t stride1_ = 0;
  int64_t stride2_ = 0;
  bool valid_ = false;

  static void CheckPositiveValid(const double param)
  {
    if (param <= 0.0)
    {
      throw std::invalid_argument("param must be positive and non-zero");
    }
    if (std::isnan(param))
    {
      throw std::invalid_argument("param must not be NaN");
    }
    if (std::isinf(param) != 0)
    {
      throw std::invalid_argument("param must not be INF");
    }
  }

  static void CheckPositiveValid(const int64_t param)
  {
    if (param <= 0)
    {
      throw std::invalid_argument("param must be positive and non-zero");
    }
  }

public:
  static uint64_t Serialize(
      const GridSizes& sizes, std::vector<uint8_t>& buffer)
  {
    return sizes.SerializeSelf(buffer);
  }

  static std::pair<GridSizes, uint64_t> Deserialize(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset)
  {
    GridSizes temp_sizes;
    const uint64_t bytes_read
        = temp_sizes.DeserializeSelf(buffer, starting_offset);
    return std::make_pair(temp_sizes, bytes_read);
  }

  GridSizes(const double cell_size,
            const double x_size,
            const double y_size,
            const double z_size)
      : GridSizes(cell_size, cell_size, cell_size, x_size, y_size, z_size) {}

  GridSizes(const double cell_size,
            const int64_t num_x_cells,
            const int64_t num_y_cells,
            const int64_t num_z_cells)
      : GridSizes(cell_size, cell_size, cell_size,
                  num_x_cells, num_y_cells, num_z_cells) {}

  GridSizes(const double cell_x_size,
            const double cell_y_size,
            const double cell_z_size,
            const double x_size,
            const double y_size,
            const double z_size)
  {
    // Safety check
    CheckPositiveValid(cell_x_size);
    CheckPositiveValid(cell_y_size);
    CheckPositiveValid(cell_z_size);
    CheckPositiveValid(x_size);
    CheckPositiveValid(y_size);
    CheckPositiveValid(z_size);
    // Set
    cell_x_size_ = cell_x_size;
    cell_y_size_ = cell_y_size;
    cell_z_size_ = cell_z_size;
    inv_cell_x_size_ = 1.0 / cell_x_size_;
    inv_cell_y_size_ = 1.0 / cell_y_size_;
    inv_cell_z_size_ = 1.0 / cell_z_size_;
    num_x_cells_
        = static_cast<int64_t>(std::ceil(x_size / cell_x_size));
    num_y_cells_
        = static_cast<int64_t>(std::ceil(y_size / cell_y_size));
    num_z_cells_
        = static_cast<int64_t>(std::ceil(z_size / cell_z_size));
    x_size_ = static_cast<double>(num_x_cells_) * cell_x_size_;
    y_size_ = static_cast<double>(num_y_cells_) * cell_y_size_;
    z_size_ = static_cast<double>(num_z_cells_) * cell_z_size_;
    stride1_ = num_y_cells_ * num_z_cells_;
    stride2_ = num_z_cells_;
    valid_ = true;
  }

  GridSizes(const double cell_x_size,
            const double cell_y_size,
            const double cell_z_size,
            const int64_t num_x_cells,
            const int64_t num_y_cells,
            const int64_t num_z_cells)
  {
    // Safety check
    CheckPositiveValid(cell_x_size);
    CheckPositiveValid(cell_y_size);
    CheckPositiveValid(cell_z_size);
    CheckPositiveValid(num_x_cells);
    CheckPositiveValid(num_y_cells);
    CheckPositiveValid(num_z_cells);
    // Set
    cell_x_size_ = cell_x_size;
    cell_y_size_ = cell_y_size;
    cell_z_size_ = cell_z_size;
    inv_cell_x_size_ = 1.0 / cell_x_size_;
    inv_cell_y_size_ = 1.0 / cell_y_size_;
    inv_cell_z_size_ = 1.0 / cell_z_size_;
    num_x_cells_ = num_x_cells;
    num_y_cells_ = num_y_cells;
    num_z_cells_ = num_z_cells;
    x_size_ = static_cast<double>(num_x_cells_) * cell_x_size_;
    y_size_ = static_cast<double>(num_y_cells_) * cell_y_size_;
    z_size_ = static_cast<double>(num_z_cells_) * cell_z_size_;
    stride1_ = num_y_cells_ * num_z_cells_;
    stride2_ = num_z_cells_;
    valid_ = true;
  }

  GridSizes()
  {
    cell_x_size_ = 0.0;
    cell_y_size_ = 0.0;
    cell_z_size_ = 0.0;
    inv_cell_x_size_ = 0.0;
    inv_cell_y_size_ = 0.0;
    inv_cell_z_size_ = 0.0;
    num_x_cells_ = 0;
    num_y_cells_ = 0;
    num_z_cells_ = 0;
    x_size_ = 0.0;
    y_size_ = 0.0;
    z_size_ = 0.0;
    stride1_ = 0.0;
    stride2_ = 0.0;
    valid_ = false;
  }

  bool Valid() const { return valid_; }

  bool UniformCellSize() const
  {
    return ((cell_x_size_ == cell_y_size_) && (cell_x_size_ == cell_z_size_));
  }

  uint64_t SerializeSelf(std::vector<uint8_t>& buffer) const
  {
    const uint64_t start_buffer_size = buffer.size();
    // Serialize everything needed to reproduce the grid sizes
    serialization::SerializeMemcpyable<double>(CellXSize(), buffer);
    serialization::SerializeMemcpyable<double>(CellYSize(), buffer);
    serialization::SerializeMemcpyable<double>(CellZSize(), buffer);
    serialization::SerializeMemcpyable<int64_t>(NumXCells(), buffer);
    serialization::SerializeMemcpyable<int64_t>(NumYCells(), buffer);
    serialization::SerializeMemcpyable<int64_t>(NumZCells(), buffer);
    // Figure out how many bytes were written
    const uint64_t end_buffer_size = buffer.size();
    const uint64_t bytes_written = end_buffer_size - start_buffer_size;
    return bytes_written;
  }

  uint64_t DeserializeSelf(const std::vector<uint8_t>& buffer,
                           const uint64_t starting_offset)
  {
    uint64_t current_position = starting_offset;
    const std::pair<double, uint64_t> cell_x_size_deserialized
        = serialization::DeserializeMemcpyable<double>(buffer,
                                                       current_position);
    const double cell_x_size = cell_x_size_deserialized.first;
    current_position += cell_x_size_deserialized.second;
    const std::pair<double, uint64_t> cell_y_size_deserialized
        = serialization::DeserializeMemcpyable<double>(buffer,
                                                       current_position);
    const double cell_y_size = cell_y_size_deserialized.first;
    current_position += cell_y_size_deserialized.second;
    const std::pair<double, uint64_t> cell_z_size_deserialized
        = serialization::DeserializeMemcpyable<double>(buffer,
                                                       current_position);
    const double cell_z_size = cell_z_size_deserialized.first;
    current_position += cell_z_size_deserialized.second;
    const std::pair<int64_t, uint64_t> num_x_cells_deserialized
        = serialization::DeserializeMemcpyable<int64_t>(buffer,
                                                        current_position);
    const int64_t num_x_cells = num_x_cells_deserialized.first;
    current_position += num_x_cells_deserialized.second;
    const std::pair<int64_t, uint64_t> num_y_cells_deserialized
        = serialization::DeserializeMemcpyable<int64_t>(buffer,
                                                        current_position);
    const int64_t num_y_cells = num_y_cells_deserialized.first;
    current_position += num_y_cells_deserialized.second;
    const std::pair<int64_t, uint64_t> num_z_cells_deserialized
        = serialization::DeserializeMemcpyable<int64_t>(buffer,
                                                        current_position);
    const int64_t num_z_cells = num_z_cells_deserialized.first;
    current_position += num_z_cells_deserialized.second;
    try
    {
      *this = GridSizes(cell_x_size, cell_y_size, cell_z_size,
                        num_x_cells, num_y_cells, num_z_cells);
    }
    catch (const std::invalid_argument&)
    {
      // The only reason the constructor can fail is if the params are
      // default-initialized or invalid. We make sure to zero and set invalid.
      *this = GridSizes();
    }
    // Figure out how many bytes were read
    const uint64_t bytes_read = current_position - starting_offset;
    return bytes_read;
  }

  double CellXSize() const { return cell_x_size_; }

  double CellYSize() const { return cell_y_size_; }

  double CellZSize() const { return cell_z_size_; }

  Eigen::Vector3d CellSizes() const
  {
    return Eigen::Vector3d(cell_x_size_, cell_y_size_, cell_z_size_);
  }

  double InvCellXSize() const { return inv_cell_x_size_; }

  double InvCellYSize() const { return inv_cell_y_size_; }

  double InvCellZSize() const { return inv_cell_z_size_; }

  int64_t NumXCells() const { return num_x_cells_; }

  int64_t NumYCells() const { return num_y_cells_; }

  int64_t NumZCells() const { return num_z_cells_; }

  Eigen::Matrix<int64_t, 3, 1> NumCells() const
  {
    Eigen::Matrix<int64_t, 3, 1> num_cells;
    num_cells << num_x_cells_, num_y_cells_, num_z_cells_;
    return num_cells;
  }

  int64_t TotalCells() const
  {
    return num_x_cells_ * num_y_cells_ * num_z_cells_;
  }

  double XSize() const { return x_size_; }

  double YSize() const { return y_size_; }

  double ZSize() const { return z_size_; }

  Eigen::Vector3d Sizes() const
  {
    return Eigen::Vector3d(x_size_, y_size_, z_size_);
  }

  int64_t Stride1() const { return stride1_; }

  int64_t Stride2() const { return stride2_; }

  bool IndexInBounds(const int64_t x_index,
                     const int64_t y_index,
                     const int64_t z_index) const
  {
    if (x_index >= 0 && y_index >= 0 && z_index >= 0
        && x_index < NumXCells() && y_index < NumYCells()
        && z_index < NumZCells())
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  bool IndexInBounds(const GridIndex& index) const
  {
    if (index.X() >= 0 && index.Y() >= 0 && index.Z() >= 0
        && index.X() < NumXCells() && index.Y() < NumYCells()
        && index.Z() < NumZCells())
    {
      return true;
    }
    else
    {
      return false;
    }
  }

  int64_t GetDataIndex(const int64_t x_index,
                       const int64_t y_index,
                       const int64_t z_index) const
  {
    return (x_index * Stride1()) + (y_index * Stride2()) + z_index;
  }

  int64_t GetDataIndex(const GridIndex& index) const
  {
    return (index.X() * Stride1()) + (index.Y() * Stride2()) + index.Z();
  }

  GridIndex GetGridIndexFromDataIndex(const int64_t data_index) const
  {
    const int64_t x_idx = data_index / Stride1();
    const int64_t remainder = data_index % Stride1();
    const int64_t y_idx = remainder / Stride2();
    const int64_t z_idx = remainder % Stride2();
    return GridIndex(x_idx, y_idx, z_idx);
  }

  Eigen::Vector4d IndexToLocationInGridFrame(const GridIndex& index) const
  {
    return IndexToLocationInGridFrame(index.X(), index.Y(), index.Z());
  }

  Eigen::Vector4d IndexToLocationInGridFrame(const int64_t x_index,
                                             const int64_t y_index,
                                             const int64_t z_index) const
  {
    const Eigen::Vector4d point_in_grid_frame(
          CellXSize() * (static_cast<double>(x_index) + 0.5),
          CellYSize() * (static_cast<double>(y_index) + 0.5),
          CellZSize() * (static_cast<double>(z_index) + 0.5),
          1.0);
    return point_in_grid_frame;
  }
};

// Forward-declare for use in GridQuery.
template<typename T, typename BackingStore=std::vector<T>>
class VoxelGridBase;

// While this looks like a std::optional<T>, it *does not own* the item of T,
// unlike std::optional<T>, since it needs to pass the caller a const/mutable
// reference to the item in the voxel grid.
template<typename T>
class GridQuery : public ReferencingMaybe<T>
{
private:
  template<typename Item, typename BackingStore> friend class VoxelGridBase;

  // This constructor is protected because users should not be able to create
  // GridQuery<T> with a value on their own, creation should only be possible
  // within a VoxelGridBase<T> to which the GridQuery<T> references.
  explicit GridQuery(T& item) : ReferencingMaybe<T>(item) {}

public:
  GridQuery() : ReferencingMaybe<T>() {}
};

/// This is the base class for all voxel grid classes. It is pure virtual to
/// force the implementation of certain necessary functions (cloning, access,
/// derived-class memeber de/serialization) in concrete implementations. This is
/// the class to inherit from if you want a VoxelGrid-like type. If all you want
/// is a voxel grid of T, see VoxelGrid<T, BackingStore> below.
template<typename T, typename BackingStore>
class VoxelGridBase
{
private:
  Eigen::Isometry3d origin_transform_ = Eigen::Isometry3d::Identity();
  Eigen::Isometry3d inverse_origin_transform_ = Eigen::Isometry3d::Identity();
  T default_value_;
  T oob_value_;
  BackingStore data_;
  GridSizes sizes_;
  bool initialized_ = false;

  T& AccessIndex(const int64_t& data_index)
  {
    if ((data_index >= 0) && (data_index < static_cast<int64_t>(data_.size())))
    {
      // Note: do not refactor to use .at(), since not all vector-like
      // implementations implement it (ex thrust::host_vector<T>).
      return data_[data_index];
    }
    else
    {
      throw std::out_of_range("data_index out of range");
    }
  }

  const T& AccessIndex(const int64_t& data_index) const
  {
    if ((data_index >= 0) && (data_index < static_cast<int64_t>(data_.size())))
    {
      // Note: do not refactor to use .at(), since not all vector-like
      // implementations implement it (ex thrust::host_vector<T>).
      return data_[data_index];
    }
    else
    {
      throw std::out_of_range("data_index out of range");
    }
  }

  void SetContents(const T& value)
  {
    data_.clear();
    data_.resize(sizes_.TotalCells(), value);
  }

  uint64_t BaseSerializeSelf(
      std::vector<uint8_t>& buffer,
      const std::function<uint64_t(
        const T&, std::vector<uint8_t>&)>& value_serializer) const
  {
    const uint64_t start_buffer_size = buffer.size();
    // Serialize the transform
    serialization::SerializeIsometry3d(origin_transform_, buffer);
    // Serialize the default value
    value_serializer(default_value_, buffer);
    // Serialize the OOB value
    value_serializer(oob_value_, buffer);
    // Serialize the data
    serialization::SerializeVectorLike<T, BackingStore>(
          data_, buffer, value_serializer);
    // Serialize the grid sizes
    GridSizes::Serialize(sizes_, buffer);
    // Serialize the initialized
    serialization::SerializeMemcpyable<uint8_t>(
        static_cast<uint8_t>(initialized_), buffer);
    // Figure out how many bytes were written
    const uint64_t end_buffer_size = buffer.size();
    const uint64_t bytes_written = end_buffer_size - start_buffer_size;
    return bytes_written;
  }

  uint64_t BaseDeserializeSelf(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset,
      const std::function<std::pair<T, uint64_t>(
        const std::vector<uint8_t>&, const uint64_t)>& value_deserializer)
  {
    uint64_t current_position = starting_offset;
    // Deserialize the transforms
    const std::pair<Eigen::Isometry3d, uint64_t> origin_transform_deserialized
        = serialization::DeserializeIsometry3d(buffer, current_position);
    origin_transform_ = origin_transform_deserialized.first;
    current_position += origin_transform_deserialized.second;
    inverse_origin_transform_ = origin_transform_.inverse();
    // Deserialize the default value
    const std::pair<T, uint64_t> default_value_deserialized
        = value_deserializer(buffer, current_position);
    default_value_ = default_value_deserialized.first;
    current_position += default_value_deserialized.second;
    // Deserialize the OOB value
    const std::pair<T, uint64_t> oob_value_deserialized
        = value_deserializer(buffer, current_position);
    oob_value_ = oob_value_deserialized.first;
    current_position += oob_value_deserialized.second;
    // Deserialize the data
    const std::pair<BackingStore, uint64_t> data_deserialized
        = serialization::DeserializeVectorLike<T, BackingStore>(
              buffer, current_position, value_deserializer);
    data_ = data_deserialized.first;
    current_position += data_deserialized.second;
    // Deserialize the cell sizes
    const std::pair<GridSizes, uint64_t> sizes_deserialized
        = GridSizes::Deserialize(buffer, current_position);
    sizes_ = sizes_deserialized.first;
    current_position += sizes_deserialized.second;
    if (sizes_.TotalCells() != static_cast<int64_t>(data_.size()))
    {
      throw std::runtime_error("sizes_.NumCells() != data_.size()");
    }
    // Deserialize the initialized
    const std::pair<uint8_t, uint64_t> initialized_deserialized
        = serialization::DeserializeMemcpyable<uint8_t>(buffer,
                                                        current_position);
    initialized_ = static_cast<bool>(initialized_deserialized.first);
    current_position += initialized_deserialized.second;
    if (sizes_.Valid() != initialized_)
    {
      throw std::runtime_error("sizes_.Valid() != initialized_");
    }
    // Figure out how many bytes were read
    const uint64_t bytes_read = current_position - starting_offset;
    return bytes_read;
  }

  bool OnMutableAccess(const GridIndex& index)
  {
    return OnMutableAccess(index.X(), index.Y(), index.Y());
  }

protected:
  // These are pure-virtual in the base clas to force their implementation in
  // derived classes.

  /// Do the work necessary for Clone() to copy the current object.
  virtual VoxelGridBase<T, BackingStore>* DoClone() const = 0;

  /// Serialize any derived-specific members into the provided buffer.
  virtual uint64_t DerivedSerializeSelf(
      std::vector<uint8_t>& buffer,
      const std::function<uint64_t(
        const T&, std::vector<uint8_t>&)>& value_serializer) const = 0;

  /// Deserialize any derived-specific members from the provided buffer.
  virtual uint64_t DerivedDeserializeSelf(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset,
      const std::function<std::pair<T, uint64_t>(
        const std::vector<uint8_t>&,
        const uint64_t)>& value_deserializer) = 0;

  /// Callback on any mutable access to the grid. Return true/false to allow or
  /// disallow access to the grid. For example, this can be used to prohibit
  /// changes to a non-const grid, or to invalidate a cache if voxels are
  /// modified.
  virtual bool OnMutableAccess(const int64_t x_index,
                               const int64_t y_index,
                               const int64_t z_index) = 0;

public:
  EIGEN_MAKE_ALIGNED_OPERATOR_NEW

  VoxelGridBase(const Eigen::Isometry3d& origin_transform,
                const GridSizes& sizes,
                const T& default_value,
                const T& oob_value)
  {
    Initialize(origin_transform, sizes, default_value, oob_value);
  }

  VoxelGridBase(const GridSizes& sizes,
                const T& default_value,
                const T& oob_value)
  {
    Initialize(sizes, default_value, oob_value);
  }

  VoxelGridBase()
  {
    origin_transform_ = Eigen::Isometry3d::Identity();
    inverse_origin_transform_ = origin_transform_.inverse();
    utility::RequireAlignment(origin_transform_, 16u);
    utility::RequireAlignment(inverse_origin_transform_, 16u);
    data_.clear();
    initialized_ = false;
  }

  virtual ~VoxelGridBase() {}

  void Initialize(const Eigen::Isometry3d& origin_transform,
                  const GridSizes& sizes,
                  const T& default_value,
                  const T& oob_value)
  {
    if (sizes.Valid())
    {
      sizes_ = sizes;
      origin_transform_ = origin_transform;
      inverse_origin_transform_ = origin_transform_.inverse();
      utility::RequireAlignment(origin_transform_, 16u);
      utility::RequireAlignment(inverse_origin_transform_, 16u);
      default_value_ = default_value;
      oob_value_ = oob_value;
      SetContents(default_value_);
      initialized_ = true;
    }
    else
    {
      throw std::invalid_argument("sizes is not valid");
    }
  }

  void Initialize(const GridSizes& sizes,
                  const T& default_value,
                  const T& oob_value)
  {
    const Eigen::Translation3d origin_translation(-sizes.XSize() * 0.5,
                                                  -sizes.YSize() * 0.5,
                                                  -sizes.ZSize() * 0.5);
    const Eigen::Isometry3d origin_transform
        = origin_translation * Eigen::Quaterniond::Identity();
    Initialize(origin_transform, sizes, default_value, oob_value);
  }

  VoxelGridBase<T, BackingStore>* Clone() const
  {
    return DoClone();
  }

  uint64_t SerializeSelf(
      std::vector<uint8_t>& buffer,
      const std::function<uint64_t(
        const T&, std::vector<uint8_t>&)>& value_serializer) const
  {
    return BaseSerializeSelf(buffer, value_serializer)
        + DerivedSerializeSelf(buffer, value_serializer);
  }

  uint64_t DeserializeSelf(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset,
      const std::function<std::pair<T, uint64_t>(
        const std::vector<uint8_t>&, const uint64_t)>& value_deserializer)
  {
    uint64_t current_position = starting_offset;
    current_position
        += BaseDeserializeSelf(buffer, starting_offset, value_deserializer);
    current_position
        += DerivedDeserializeSelf(buffer, current_position, value_deserializer);
    // Figure out how many bytes were read
    const uint64_t bytes_read = current_position - starting_offset;
    return bytes_read;
  }

  bool IsInitialized() const
  {
    return initialized_;
  }

  void ResetWithDefault()
  {
    SetContents(default_value_);
  }

  void ResetWithNewValue(const T& new_value)
  {
    SetContents(new_value);
  }

  void ResetWithNewDefault(const T& new_default)
  {
    default_value_ = new_default;
    SetContents(default_value_);
  }

  bool IndexInBounds(const int64_t x_index,
                     const int64_t y_index,
                     const int64_t z_index) const
  {
    return sizes_.IndexInBounds(x_index, y_index, z_index);
  }

  bool IndexInBounds(const GridIndex& index) const
  {
    return sizes_.IndexInBounds(index);
  }

  bool LocationInBounds(const double x,
                        const double y,
                        const double z) const
  {
    const GridIndex index = LocationToGridIndex(x, y, z);
    return sizes_.IndexInBounds(index);
  }

  bool LocationInBounds3d(const Eigen::Vector3d& location) const
  {
    const GridIndex index = LocationToGridIndex3d(location);
    return sizes_.IndexInBounds(index);
  }

  bool LocationInBounds4d(const Eigen::Vector4d& location) const
  {
    const GridIndex index = LocationToGridIndex4d(location);
    return sizes_.IndexInBounds(index);
  }

  GridQuery<const T> GetImmutable3d(const Eigen::Vector3d& location) const
  {
    return GetImmutable(LocationToGridIndex3d(location));
  }

  GridQuery<const T> GetImmutable4d(const Eigen::Vector4d& location) const
  {
    return GetImmutable(LocationToGridIndex4d(location));
  }

  GridQuery<const T> GetImmutable(const double x,
                                  const double y,
                                  const double z) const
  {
    const Eigen::Vector4d location(x, y, z, 1.0);
    return GetImmutable4d(location);
  }

  GridQuery<const T> GetImmutable(const GridIndex& index) const
  {
    if (sizes_.IndexInBounds(index))
    {
      return GridQuery<const T>(AccessIndex(sizes_.GetDataIndex(index)));
    }
    else
    {
      return GridQuery<const T>();
    }
  }

  GridQuery<const T> GetImmutable(const int64_t x_index,
                                  const int64_t y_index,
                                  const int64_t z_index) const
  {
    if (sizes_.IndexInBounds(x_index, y_index, z_index))
    {
      return GridQuery<const T>(
            AccessIndex(sizes_.GetDataIndex(x_index, y_index, z_index)));
    }
    else
    {
      return GridQuery<const T>();
    }
  }

  GridQuery<T> GetMutable3d(const Eigen::Vector3d& location)
  {
    return GetMutable(LocationToGridIndex3d(location));
  }

  GridQuery<T> GetMutable4d(const Eigen::Vector4d& location)
  {
    return GetMutable(LocationToGridIndex4d(location));
  }

  GridQuery<T> GetMutable(const double x,
                          const double y,
                          const double z)
  {
    const Eigen::Vector4d location(x, y, z, 1.0);
    return GetMutable4d(location);
  }

  GridQuery<T> GetMutable(const GridIndex& index)
  {
    if (sizes_.IndexInBounds(index) && OnMutableAccess(index))
    {
      return GridQuery<T>(AccessIndex(sizes_.GetDataIndex(index)));
    }
    else
    {
      return GridQuery<T>();
    }
  }

  GridQuery<T> GetMutable(const int64_t x_index,
                          const int64_t y_index,
                          const int64_t z_index)
  {
    if (sizes_.IndexInBounds(x_index, y_index, z_index) &&
        OnMutableAccess(x_index, y_index, z_index))
    {
      return GridQuery<T>(
            AccessIndex(sizes_.GetDataIndex(x_index, y_index, z_index)));
    }
    else
    {
      return GridQuery<T>();
    }
  }

  bool SetValue3d(const Eigen::Vector3d& location, const T& value)
  {
    return SetValue(LocationToGridIndex3d(location), value);
  }

  bool SetValue4d(const Eigen::Vector4d& location, const T& value)
  {
    return SetValue(LocationToGridIndex4d(location), value);
  }

  bool SetValue(const double x,
                const double y,
                const double z,
                const T& value)
  {
    const Eigen::Vector4d location(x, y, z, 1.0);
    return SetValue4d(location, value);
  }

  bool SetValue(const GridIndex& index, const T& value)
  {
    if (sizes_.IndexInBounds(index) && OnMutableAccess(index))
    {
      AccessIndex(sizes_.GetDataIndex(index)) = value;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool SetValue(const int64_t x_index,
                const int64_t y_index,
                const int64_t z_index,
                const T& value)
  {
    if (sizes_.IndexInBounds(x_index, y_index, z_index) &&
        OnMutableAccess(x_index, y_index, z_index))
    {
      AccessIndex(sizes_.GetDataIndex(x_index, y_index, z_index)) = value;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool SetValue3d(const Eigen::Vector3d& location, T&& value)
  {
    return SetValue(LocationToGridIndex3d(location), value);
  }

  bool SetValue4d(const Eigen::Vector4d& location, T&& value)
  {
    return SetValue(LocationToGridIndex4d(location), value);
  }

  bool SetValue(const double x,
                const double y,
                const double z,
                T&& value)
  {
    const Eigen::Vector4d location(x, y, z, 1.0);
    return SetValue4d(location, value);
  }

  bool SetValue(const GridIndex& index, T&& value)
  {
    if (sizes_.IndexInBounds(index) && OnMutableAccess(index))
    {
      AccessIndex(sizes_.GetDataIndex(index)) = value;
      return true;
    }
    else
    {
      return false;
    }
  }

  bool SetValue(const int64_t x_index,
                const int64_t y_index,
                const int64_t z_index,
                T&& value)
  {
    if (sizes_.IndexInBounds(x_index, y_index, z_index) &&
        OnMutableAccess(x_index, y_index, z_index))
    {
      AccessIndex(sizes_.GetDataIndex(x_index, y_index, z_index)) = value;
      return true;
    }
    else
    {
      return false;
    }
  }

  const GridSizes& GetGridSizes() const { return sizes_; }

  double GetXSize() const { return sizes_.XSize(); }

  double GetYSize() const { return sizes_.YSize(); }

  double GetZSize() const { return sizes_.ZSize(); }

  Eigen::Vector3d GetSizes() const { return sizes_.Sizes(); }

  Eigen::Vector3d GetCellSizes() const { return sizes_.CellSizes(); }

  bool HasUniformCellSize() const { return sizes_.UniformCellSize(); }

  const T& GetDefaultValue() const { return default_value_; }

  const T& GetOOBValue() const { return oob_value_; }

  void SetDefaultValue(const T& default_value)
  {
    default_value_ = default_value;
  }

  void SetOOBValue(const T& oob_value) { oob_value_ = oob_value; }

  int64_t GetTotalCells() const { return sizes_.TotalCells(); }

  int64_t GetNumXCells() const { return sizes_.NumXCells(); }

  int64_t GetNumYCells() const { return sizes_.NumYCells(); }

  int64_t GetNumZCells() const { return sizes_.NumZCells(); }

  Eigen::Matrix<int64_t, 3, 1> GetNumCells() const { return sizes_.NumCells(); }

  const Eigen::Isometry3d& GetOriginTransform() const
  {
    return origin_transform_;
  }

  const Eigen::Isometry3d& GetInverseOriginTransform() const
  {
    return inverse_origin_transform_;
  }

  void UpdateOriginTransform(const Eigen::Isometry3d& origin_transform)
  {
    origin_transform_ = origin_transform;
    inverse_origin_transform_ = origin_transform_.inverse();
  }

  GridIndex LocationInGridFrameToGridIndex(const double x,
                                           const double y,
                                           const double z) const
  {
    return LocationInGridFrameToGridIndex4d(Eigen::Vector4d(x, y, z, 1.0));
  }

  GridIndex LocationInGridFrameToGridIndex3d(
      const Eigen::Vector3d& location) const
  {
    const int64_t x_cell
        = static_cast<int64_t>(location.x() * sizes_.InvCellXSize());
    const int64_t y_cell
        = static_cast<int64_t>(location.y() * sizes_.InvCellYSize());
    const int64_t z_cell
        = static_cast<int64_t>(location.z() * sizes_.InvCellZSize());
    return GridIndex(x_cell, y_cell, z_cell);
  }

  GridIndex LocationInGridFrameToGridIndex4d(
      const Eigen::Vector4d& location) const
  {
    if (location(3) == 1.0)
    {
      const int64_t x_cell
          = static_cast<int64_t>(location(0) * sizes_.InvCellXSize());
      const int64_t y_cell
          = static_cast<int64_t>(location(1) * sizes_.InvCellYSize());
      const int64_t z_cell
          = static_cast<int64_t>(location(2) * sizes_.InvCellZSize());
      return GridIndex(x_cell, y_cell, z_cell);
    }
    else
    {
      throw std::invalid_argument("location(3) != 1");
    }
  }

  GridIndex LocationToGridIndex(const double x,
                                const double y,
                                const double z) const
  {
    return LocationToGridIndex4d(Eigen::Vector4d(x, y, z, 1.0));
  }

  GridIndex LocationToGridIndex3d(const Eigen::Vector3d& location) const
  {
    const Eigen::Vector3d point_in_grid_frame
        = GetInverseOriginTransform() * location;
    return LocationInGridFrameToGridIndex3d(point_in_grid_frame);
  }

  GridIndex LocationToGridIndex4d(const Eigen::Vector4d& location) const
  {
    const Eigen::Vector4d point_in_grid_frame
        = GetInverseOriginTransform() * location;
    return LocationInGridFrameToGridIndex4d(point_in_grid_frame);
  }

  Eigen::Vector4d GridIndexToLocationInGridFrame(const GridIndex& index) const
  {
    return sizes_.IndexToLocationInGridFrame(index);
  }

  Eigen::Vector4d GridIndexToLocationInGridFrame(const int64_t x_index,
                                                 const int64_t y_index,
                                                 const int64_t z_index) const
  {
    return sizes_.IndexToLocationInGridFrame(x_index, y_index, z_index);
  }

  Eigen::Vector4d GridIndexToLocation(const GridIndex& index) const
  {
    return GetOriginTransform() * GridIndexToLocationInGridFrame(index);
  }

  Eigen::Vector4d GridIndexToLocation(const int64_t x_index,
                                      const int64_t y_index,
                                      const int64_t z_index) const
  {
    return GetOriginTransform()
        * GridIndexToLocationInGridFrame(x_index, y_index, z_index);
  }

  int64_t GridIndexToDataIndex(const int64_t x_index,
                               const int64_t y_index,
                               const int64_t z_index) const
  {
    return sizes_.GetDataIndex(x_index, y_index, z_index);
  }

  int64_t GridIndexToDataIndex(const GridIndex& index) const
  {
    return sizes_.GetDataIndex(index);
  }

  GridIndex DataIndexToGridIndex(const int64_t data_index) const
  {
    return sizes_.GetGridIndexFromDataIndex(data_index);
  }

  BackingStore& GetMutableRawData() { return data_; }

  const BackingStore& GetImmutableRawData() const { return data_; }

  bool SetRawData(const BackingStore& data)
  {
    const int64_t expected_length = GetTotalCells();
    if ((int64_t)data.size() == expected_length)
    {
      data_ = data;
      return true;
    }
    else
    {
      std::cerr << "Failed to load internal data - expected "
                << expected_length << " got " << data.size() << std::endl;
      return false;
    }
  }

  uint64_t HashDataIndex(const int64_t x_index,
                         const int64_t y_index,
                         const int64_t z_index) const
  {
    return static_cast<uint64_t>(
        GridIndexToDataIndex(x_index, y_index, z_index));
  }

  uint64_t HashDataIndex(const GridIndex& index) const
  {
    return static_cast<uint64_t>(GridIndexToDataIndex(index));
  }
};

/// If you want a VoxelGrid<T> this is the class to use. Since you should never
/// inherit from it, this class is final.
template<typename T, typename BackingStore=std::vector<T>>
class VoxelGrid final : public VoxelGridBase<T, BackingStore>
{
private:
  VoxelGridBase<T, BackingStore>* DoClone() const override
  {
    return new VoxelGrid<T, BackingStore>(
        static_cast<const VoxelGrid<T, BackingStore>&>(*this));
  }

  uint64_t DerivedSerializeSelf(
      std::vector<uint8_t>& buffer,
      const std::function<uint64_t(
        const T&, std::vector<uint8_t>&)>& value_serializer) const override
  {
    UNUSED(buffer);
    UNUSED(value_serializer);
    return 0;
  }

  uint64_t DerivedDeserializeSelf(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset,
      const std::function<std::pair<T, uint64_t>(
        const std::vector<uint8_t>&,
        const uint64_t)>& value_deserializer) override
  {
    UNUSED(buffer);
    UNUSED(starting_offset);
    UNUSED(value_deserializer);
    return 0;
  }

  bool OnMutableAccess(const int64_t x_index,
                       const int64_t y_index,
                       const int64_t z_index) override
  {
    UNUSED(x_index);
    UNUSED(y_index);
    UNUSED(z_index);
    return true;
  }

public:
  static uint64_t Serialize(
      const VoxelGrid<T, BackingStore>& grid, std::vector<uint8_t>& buffer,
      const std::function<uint64_t(
        const T&, std::vector<uint8_t>&)>& value_serializer)
  {
    return grid.SerializeSelf(buffer, value_serializer);
  }

  static std::pair<VoxelGrid<T, BackingStore>, uint64_t> Deserialize(
      const std::vector<uint8_t>& buffer, const uint64_t starting_offset,
      const std::function<std::pair<T, uint64_t>(
        const std::vector<uint8_t>&, const uint64_t)>& value_deserializer)
  {
    VoxelGrid<T, BackingStore> temp_grid;
    const uint64_t bytes_read
        = temp_grid.DeserializeSelf(buffer, starting_offset,
                                    value_deserializer);
    return std::make_pair(temp_grid, bytes_read);
  }

  VoxelGrid(const Eigen::Isometry3d& origin_transform,
            const GridSizes& sizes,
            const T& default_value)
      : VoxelGridBase<T, BackingStore>(
          origin_transform, sizes, default_value, default_value) {}

  VoxelGrid(const Eigen::Isometry3d& origin_transform,
            const GridSizes& sizes,
            const T& default_value,
            const T& oob_value)
      : VoxelGridBase<T, BackingStore>(
          origin_transform, sizes, default_value, oob_value) {}

  VoxelGrid(const GridSizes& sizes, const T& default_value)
      : VoxelGridBase<T, BackingStore>(sizes, default_value, default_value) {}

  VoxelGrid(const GridSizes& sizes, const T& default_value, const T& oob_value)
      : VoxelGridBase<T, BackingStore>(sizes, default_value, oob_value) {}

  VoxelGrid() : VoxelGridBase<T, BackingStore>() {}
};
}  // namespace voxel_grid
}  // namespace common_robotics_utilities

namespace std
{
template <>
struct hash<common_robotics_utilities::voxel_grid::GridIndex>
{
  std::size_t operator()(
      const common_robotics_utilities::voxel_grid::GridIndex& index) const
  {
    std::size_t hash_val = 0;
    common_robotics_utilities::utility::hash_combine(
        hash_val, index.X(), index.Y(), index.Z());
    return hash_val;
  }
};
}

inline std::ostream& operator<<(
    std::ostream& strm,
    const common_robotics_utilities::voxel_grid::GridIndex& index)
{
  strm << "GridIndex: (" << index.X() << "," << index.Y() << "," << index.Z()
       << ")";
  return strm;
}
