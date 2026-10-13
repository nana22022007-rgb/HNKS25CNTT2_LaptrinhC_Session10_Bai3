#include <stdio.h>
#include <string.h>

typedef enum {
    SUCCESS = 0,
    ERR_INVALID_QTY = -1,
    ERR_OVERBOOKING = -2,
    ERR_ZONE_NOT_FOUND = -3,
    ERR_QR_USED = -4,
    ERR_QR_NOT_FOUND = -5,
    ERR_EMPTY_LIST = -6
} ErrorCode;

typedef struct {
    int zone_id;
    char zone_name[50];
    double ticket_price;
    int total_capacity;
    int sold_tickets;
} TicketZone;

typedef struct {
    char qr_code[50];
    int zone_id;
    int is_used;
} QrCheckIn;

int find_zone_index(TicketZone zones[], int total_zones, int zone_id) {
    if (total_zones <= 0) return ERR_EMPTY_LIST;
    for (int i = 0; i < total_zones; i++) {
        if (zones[i].zone_id == zone_id) {
            return i;
        }
    }
    return -1;
}

int find_qr_index(QrCheckIn qrs[], int total_qrs, const char* qr_code) {
    if (total_qrs <= 0) return ERR_EMPTY_LIST;
    for (int i = 0; i < total_qrs; i++) {
        if (strcmp(qrs[i].qr_code, qr_code) == 0) {
            return i;
        }
    }
    return -1;
}

ErrorCode process_ticket_sale(TicketZone zones[], int total_zones, int zone_id, int quantity, int is_early_bird, double* final_price) {
    if (total_zones <= 0) return ERR_EMPTY_LIST;

    if (quantity < 1 || quantity > 4) {
        return ERR_INVALID_QTY;
    }

    int idx = find_zone_index(zones, total_zones, zone_id);
    if (idx == -1) {
        return ERR_ZONE_NOT_FOUND;
    }

    if (zones[idx].sold_tickets + quantity > zones[idx].total_capacity) {
        return ERR_OVERBOOKING;
    }

    double price_per_ticket = zones[idx].ticket_price;
    if (is_early_bird == 1) {
        price_per_ticket *= 0.85;
    }

    *final_price = price_per_ticket * quantity;
    zones[idx].sold_tickets += quantity;

    return SUCCESS;
}

ErrorCode process_qr_checkin(QrCheckIn qrs[], int total_qrs, TicketZone zones[], int total_zones, const char* qr_code) {
    if (total_qrs <= 0 || total_zones <= 0) return ERR_EMPTY_LIST;

    int qr_idx = find_qr_index(qrs, total_qrs, qr_code);
    if (qr_idx == -1) {
        return ERR_QR_NOT_FOUND;
    }

    if (qrs[qr_idx].is_used == 1) {
        return ERR_QR_USED;
    }

    qrs[qr_idx].is_used = 1;
    return SUCCESS;
}

void print_error_message(ErrorCode code) {
    switch (code) {
        case SUCCESS:
            printf("Giao dich duoc xu ly hoan tat.\n");
            break;
        case ERR_INVALID_QTY:
            printf("So luong mua khong hop le (Phai tu 1 den 4 ve).\n");
            break;
        case ERR_OVERBOOKING:
            printf("Tran dung luong khu vuc (Vuot qua suc chua con lai).\n");
            break;
        case ERR_ZONE_NOT_FOUND:
            printf("Ma khu vuc khong ton tai trong he thong.\n");
            break;
        case ERR_QR_USED:
            printf("Ma QR da duoc su dung truoc do (Double Check-in bi tu choi).\n");
            break;
        case ERR_QR_NOT_FOUND:
            printf("Ma QR khong hop le hoac khong ton tai.\n");
            break;
        case ERR_EMPTY_LIST:
            printf("Danh sach du lieu rong.\n");
            break;
        default:
            printf("Loi khong xac dinh.\n");
            break;
    }
}

int main() {
    TicketZone zones[2] = {
        {101, "VIP Zone", 2000000.0, 5, 3},
        {102, "Standard Zone", 1000000.0, 10, 8}
    };
    int total_zones = 2;

    QrCheckIn qrs[2] = {
        {"QR_VIP_001", 101, 0},
        {"QR_STD_002", 102, 1}
    };
    int total_qrs = 2;

    double final_price = 0.0;
    ErrorCode result;

    printf("=== MO PHONG KICH BAN KIEM THU (TICKETBOX LIVE CONCERT) ===\n\n");

    printf("Test Case 1: Mua 5 ve VIP (Vuot han muc toi da 4 ve)\n");
    result = process_ticket_sale(zones, total_zones, 101, 5, 0, &final_price);
    print_error_message(result);
    printf("--------------------------------------------------\n");

    printf("Test Case 2: Mua 3 ve Standard (Khu vuc chi con 2 cho trong)\n");
    result = process_ticket_sale(zones, total_zones, 102, 3, 0, &final_price);
    print_error_message(result);
    printf("--------------------------------------------------\n");

    printf("Test Case 3: Mua 2 ve VIP, Early Bird = 1\n");
    result = process_ticket_sale(zones, total_zones, 101, 2, 1, &final_price);
    print_error_message(result);
    if (result == SUCCESS) {
        printf("-> Tong tien thanh toan: %.2f VND\n", final_price);
    }
    printf("--------------------------------------------------\n");

    printf("Test Case 4: Quet check-in ma 'QR_STD_002' (Da su dung truoc do)\n");
    result = process_qr_checkin(qrs, total_qrs, zones, total_zones, "QR_STD_002");
    print_error_message(result);
    printf("--------------------------------------------------\n");

    printf("Test Case 5: Quet check-in ma 'QR_VIP_001' (Chua su dung)\n");
    result = process_qr_checkin(qrs, total_qrs, zones, total_zones, "QR_VIP_001");
    print_error_message(result);
    printf("Trang thai moi cua QR_VIP_001 (is_used): %d\n", qrs[0].is_used);
    printf("--------------------------------------------------\n");

    return 0;
}
